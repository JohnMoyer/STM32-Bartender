/*
 * stepper.c
 *
 *  Created on: Mar 3, 2026
 *      Author: moyerjf
 */

#include "stm32f103x6.h"
#include "delay.h"

/*
 * Convert to use interrupts from timer 2. 8MHz clock.
 */

#define STEPS_PER_REVOLUTION 1600
#define STEPS_PER_MMX100 3400
#define STEP_PULSE_US 5

void stepperInit(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;  					// enable GPIOA clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    GPIOA->CRL &= ~(GPIO_CRL_CNF3 | GPIO_CRL_MODE3);
    GPIOA->CRL |= GPIO_CRL_MODE3_0 | GPIO_CRL_MODE3_1;

    TIM2->PSC = 8 - 1;
    TIM2->ARR = 1000 - 1;
    TIM2->DIER |= TIM_DIER_UIE_Msk;
    TIM2->CR1 |= TIM_CR1_CEN_Msk;

    NVIC->ISER[0] |= (1 << TIM2_IRQn);

    // PA0 (DIR), PA1 (STEP), PA2 (ENABLE) as output push-pull 50MHz
    GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0 |
                    GPIO_CRL_CNF1 | GPIO_CRL_MODE1 |
					GPIO_CRL_CNF2 | GPIO_CRL_MODE2);
    GPIOA->CRL |= (GPIO_CRL_MODE0_0 | GPIO_CRL_MODE0_1 |
                   GPIO_CRL_MODE1_0 | GPIO_CRL_MODE1_1 |
				   GPIO_CRL_MODE2_0 | GPIO_CRL_MODE2_1);

    GPIOA->ODR &= ~(GPIO_ODR_ODR0_Msk |
    				GPIO_ODR_ODR1_Msk |
					GPIO_ODR_ODR2_Msk);
    GPIOA->ODR |= GPIO_ODR_ODR2_Msk;   						// EN high (disable driver)

    GPIOA->BSRR = GPIO_BSRR_BS0;   							// DIR high - CCW when looking at motor head-on
}


static void step(void) {
    GPIOA->BSRR = GPIO_BSRR_BS1;   							// STEP high
    delayStkUs(STEP_PULSE_US);
    GPIOA->BSRR = GPIO_BSRR_BR1;   							// STEP low
    delayStkUs(STEP_PULSE_US);
}

static void runSteps(uint32_t steps, uint32_t delayUs)
{
    GPIOA->BSRR = GPIO_BSRR_BR2;
    delayStkUs(100);

    for(uint32_t i=0;i<steps;i++){
        step();
        delayStkUs(delayUs);
    }

    GPIOA->BSRR = GPIO_BSRR_BS2;
}


void spinDegrees(uint16_t degrees, uint32_t tMS, uint8_t dir) {
	//Dir: 1 is CCW, 0 is CW
	GPIOA->BSRR = dir ? GPIO_BSRR_BS0 : GPIO_BSRR_BR0;

	uint32_t steps = (degrees * STEPS_PER_REVOLUTION) / 360;
	if (steps == 0) return;

	uint32_t delayUs = (tMS * 1000) / steps;

	runSteps(steps, delayUs);                         	// EN high (disable driver)
}


void moveMM(uint16_t MM, uint32_t tMS, uint8_t dir) {
															//Dir: 1 is CCW, 0 is CW
	GPIOA->BSRR = dir ? GPIO_BSRR_BS0 : GPIO_BSRR_BR0;

	uint32_t steps = (MM * STEPS_PER_MMX100) / 100;
	if (steps == 0) return;

	uint32_t delayUs = (tMS * 1000) / steps;

	runSteps(steps, delayUs);
}


void spinContinuous(void) {
	GPIOA->BSRR = GPIO_BSRR_BR2;   							// EN low (enable driver)
	for(;;) {
		step();
		delayStkUs(3000);										// speed control
	}														// EN high (disable driver)
}

void TIM2_IRQHandler(void) {
	if (TIM2->SR & TIM_SR_UIF) {
		TIM2->SR &= ~TIM_SR_UIF;
		GPIOA->ODR ^= GPIO_ODR_ODR3;
	}
}
