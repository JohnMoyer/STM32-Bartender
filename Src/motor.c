/*
 * motor.c
 *
 *  Created on: Feb 20, 2026
 *      Author: moyerjf
 */


#include "stm32f103xb.h"
#include "motor.h"

volatile uint8_t active_motor = 0xFE;	//0xFE is startup state

void motor_control_init() {
	//Output pins PA3 and PA4 for pump motors
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN_Msk;
	GPIOA->CRL &= ~(GPIO_CRL_CNF3 | GPIO_CRL_MODE3 |
	                GPIO_CRL_CNF4 | GPIO_CRL_MODE4);
	GPIOA->CRL |= (GPIO_CRL_MODE3_0 | GPIO_CRL_MODE3_1 |
	               GPIO_CRL_MODE4_0 | GPIO_CRL_MODE4_1);

    TIM3->PSC = 8000 - 1;
    TIM3->DIER |= TIM_DIER_UIE_Msk;
    TIM3->CR1 &= ~TIM_CR1_CEN_Msk;

    NVIC_EnableIRQ(TIM3_IRQn);

    motor_off(0);
    motor_off(1);
}

void motor_on(uint8_t motorIndex) {
	switch (motorIndex) {
	case 0:
		GPIOA->BSRR = GPIO_BSRR_BS3;
		break;
	case 1:
		GPIOA->BSRR = GPIO_BSRR_BS4;
		break;
	}
}

void motor_off(uint8_t motorIndex) {
	switch (motorIndex) {
	case 0:
		GPIOA->BSRR = GPIO_BSRR_BR3;
		break;
	case 1:
		GPIOA->BSRR = GPIO_BSRR_BR4;
		break;
	}
}

void motor_run_ms(uint8_t motorIndex, uint32_t ms) {
	active_motor = motorIndex;

	motor_on(motorIndex);

    TIM3->ARR = ms - 1;								//1ms period for TIM3

    TIM3->CNT = 0;
    TIM3->SR &= ~TIM_SR_UIF;
    TIM3->CR1 |= TIM_CR1_CEN_Msk;
}

void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF;
        TIM3->CR1 &= ~TIM_CR1_CEN;
        if (active_motor != 0xFF) {
            motor_off(active_motor);
            active_motor = 0xFF;
        }
        // Re-enable buttons
        EXTI->PR  = EXTI_PR_PR8 | EXTI_PR_PR9;
        EXTI->IMR |= EXTI_IMR_MR8 | EXTI_IMR_MR9;
    }
}
