/*
 * stepper.c
 *
 *  Created on: Mar 3, 2026
 *      Author: moyerjf
 */

#include "stm32f103xb.h"
#include "delay.h"
#include "selector.h"

/*
 * Convert to use interrupts from timer 2. 8MHz clock.
 */

typedef enum {
	STEPPER_IDLE,
	STEPPER_RAMP_UP,
	STEPPER_CRUISE,
	STEPPER_RAMP_DOWN
} StepperState;

#define STEPS_PER_REV_CONVEYOR 1600
#define STEPS_PER_REV_PUMP 200
#define STEPS_PER_MMX100 4038
#define POUR_ARR 1799
#define STEP_PULSE_US 5

volatile uint32_t steps_remaining = 0;
volatile uint32_t steps_total	 = 0;
static volatile uint32_t ramp_steps 	 = 0;
static volatile uint32_t min_arr 		 = 0;			    //fastest
static volatile uint32_t max_arr 		 = 0;				//slowest
static volatile StepperState state 		 = STEPPER_IDLE;
volatile uint8_t stepper_done 			 = 0;


void stepperInit(void) {
	initSelector();


    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;  					// enable GPIOA clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    GPIOA->CRL &= ~(GPIO_CRL_CNF3 | GPIO_CRL_MODE3);
    GPIOA->CRL |= GPIO_CRL_MODE3_0 | GPIO_CRL_MODE3_1;

    TIM2->PSC = 8 - 1;
    TIM2->ARR = 1000 - 1;									//Default 1ms period, overwritten later
    TIM2->DIER |= TIM_DIER_UIE_Msk;
    TIM2->CR1 &= ~TIM_CR1_CEN_Msk;

    TIM2->SR = 0;
    NVIC_ClearPendingIRQ(TIM2_IRQn);
    stepper_done = 0;
    steps_remaining = 0;
    steps_total = 0;

    NVIC->ISER[0] |= (1 << TIM2_IRQn);

    // PA0 (DIR), PA1 (STEP), PA2 (ENABLE) as output push-pull 50MHz
    GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0 |
                    GPIO_CRL_CNF1 | GPIO_CRL_MODE1 );
    GPIOA->CRL |= (GPIO_CRL_MODE0_0 | GPIO_CRL_MODE0_1 |
                   GPIO_CRL_MODE1_0 | GPIO_CRL_MODE1_1);

    GPIOA->ODR &= ~(GPIO_ODR_ODR0_Msk |
    				GPIO_ODR_ODR1_Msk );

    GPIOA->BSRR = GPIO_BSRR_BS0;   							// DIR high - CCW when looking at motor head-on

}


static void startMove(uint32_t steps, uint32_t delayUs, uint8_t dir) {
	if (steps == 0) return;

	GPIOA->BSRR = dir ? GPIO_BSRR_BR0 : GPIO_BSRR_BS0;

	EXTI->IMR &= ~(EXTI_IMR_MR8 | EXTI_IMR_MR9);			//turn off buttons while moving

	steps_total = steps;
	steps_remaining = steps;
	ramp_steps = steps / 10;
	if (ramp_steps < 1) ramp_steps = 1;

	min_arr = delayUs - 1;									//cruise ARR
	max_arr = (delayUs * 4) - 1;							//slow ARR

	state = STEPPER_RAMP_UP;
	stepper_done = 0;

	GPIOA->BSRR = GPIO_BSRR_BR2;							//EN low (enable driver)

	TIM2->ARR = max_arr;
	TIM2->CNT = 0;
	TIM2->CR1 |= TIM_CR1_CEN_Msk;

    TIM3->PSC = 8000 - 1;
    TIM3->EGR  = TIM_EGR_UG;   // latch PSC immediately
    TIM3->SR   = 0;             // clear flag set by EGR
    TIM3->DIER |= TIM_DIER_UIE_Msk;
    TIM3->CR1 &= ~TIM_CR1_CEN_Msk;

    NVIC_EnableIRQ(TIM3_IRQn);
}

static void startPour(uint32_t steps, uint8_t dir) {
    if (steps == 0) return;

    GPIOA->BSRR = dir ? GPIO_BSRR_BS0 : GPIO_BSRR_BR0;

    EXTI->IMR &= ~(EXTI_IMR_MR8 | EXTI_IMR_MR9);

    steps_total     = steps;
    steps_remaining = steps;
    ramp_steps      = 1;        //no ramp

//    min_arr = delayUs - 1;
//    max_arr = delayUs - 1;      //same as min, no ramp range needed
    min_arr = POUR_ARR;
    max_arr = POUR_ARR;

    state        = STEPPER_CRUISE;  //start directly in cruise
    stepper_done = 0;

    GPIOA->BSRR = GPIO_BSRR_BR2;

    TIM2->ARR = POUR_ARR;
    TIM2->CNT = 0;
    TIM2->CR1 |= TIM_CR1_CEN_Msk;

    TIM3->PSC = 8000 - 1;
    TIM3->EGR  = TIM_EGR_UG;   // latch PSC immediately
    TIM3->SR   = 0;             // clear flag set by EGR
    TIM3->DIER |= TIM_DIER_UIE_Msk;
    TIM3->CR1 &= ~TIM_CR1_CEN_Msk;

    NVIC_EnableIRQ(TIM3_IRQn);
}

void moveMM(uint16_t MM, uint32_t tMS, uint8_t dir) {
	selectStepper(0);
	enableSelector();
	uint32_t steps = (MM * STEPS_PER_MMX100) / 100;
	uint32_t delayUs = (tMS * 1000) / steps;
	startMove(steps, delayUs, dir);
}

void spinDegrees(uint16_t degrees, uint32_t tMS, uint8_t dir) {
	uint32_t steps = (degrees * STEPS_PER_REV_CONVEYOR) / 360;
	uint32_t delayUs = (tMS * 1000) / steps;
	startMove(steps, delayUs, dir);
}

void pourDrink(uint8_t motor, uint32_t ms, uint8_t dir) {
	selectStepper(motor);
	enableSelector();
	startPour(0xFFFFFF, dir);

	TIM3->ARR = ms - 1;
	TIM3->CNT = 0;
	TIM3->SR = 0;
	TIM3->EGR = TIM_EGR_UG;  	 			//force update to latch PSC and ARR
	TIM3->SR  = 0;             				//clear the update flag EGR just set
	TIM3->CR1 |= TIM_CR1_CEN_Msk;
}

void TIM2_IRQHandler(void) {
    if (!(TIM2->SR & TIM_SR_UIF)) return;
    TIM2->SR &= ~TIM_SR_UIF;

    if (steps_remaining == 0  && state != STEPPER_CRUISE) {
        TIM2->CR1 &= ~TIM_CR1_CEN_Msk;     // stop timer
        GPIOA->BSRR = GPIO_BSRR_BS2;       // EN high (disable)
        state        = STEPPER_IDLE;
        stepper_done = 1;
        return;
    }

    // Generate step pulse
    GPIOA->BSRR = GPIO_BSRR_BS1;
    delayStkUs(5);  // ~5us pulse at 8MHz
    GPIOA->BSRR = GPIO_BSRR_BR1;

    uint32_t steps_done = steps_total - steps_remaining;
    steps_remaining--;

    // Update ARR for trapezoidal profile
    if (steps_done < ramp_steps) {
        // Ramp up
        uint32_t arr = max_arr - ((max_arr - min_arr) * steps_done / ramp_steps);
        TIM2->ARR = arr;
        state = STEPPER_RAMP_UP;
    } else if (steps_remaining < ramp_steps) {
        // Ramp down
        uint32_t arr = max_arr - ((max_arr - min_arr) * steps_remaining / ramp_steps);
        TIM2->ARR = arr;
        state = STEPPER_RAMP_DOWN;
    } else {
        // Cruise
        TIM2->ARR = min_arr;
        state = STEPPER_CRUISE;
    }

    if (steps_remaining == 0 && state != STEPPER_CRUISE) {								//Move is done
        TIM2->CR1 &= ~TIM_CR1_CEN_Msk;
        GPIOA->BSRR = GPIO_BSRR_BS2;
        state        = STEPPER_IDLE;
        stepper_done = 1;
        disableSelector();

        // Re-enable buttons
        EXTI->PR  = EXTI_PR_PR8 | EXTI_PR_PR9;   // clear any pending
        EXTI->IMR |= EXTI_IMR_MR8 | EXTI_IMR_MR9;
        return;
    }
}
void TIM3_IRQHandler(void) {
    if (!(TIM3->SR & TIM_SR_UIF)) return;
    TIM3->SR = ~TIM_SR_UIF;

    TIM3->CR1 &= ~TIM_CR1_CEN_Msk;
    TIM2->CR1 &= ~TIM_CR1_CEN_Msk;

    GPIOA->BSRR = GPIO_BSRR_BR1;  // STEP low
    GPIOA->BSRR = GPIO_BSRR_BS2;  // disable driver

    disableSelector();

    state        = STEPPER_IDLE;
    stepper_done = 1;

    EXTI->PR  = EXTI_PR_PR8 | EXTI_PR_PR9;
    EXTI->IMR |= EXTI_IMR_MR8 | EXTI_IMR_MR9;
}
