/*
 * motor.c
 *
 *  Created on: Feb 20, 2026
 *      Author: moyerjf
 */

//Using Sparkfun TB303A1 motor driver to drive Ice Auger
//https://www.sparkfun.com/sparkfun-motor-driver-dual-tb6612fng-1a.html

/*
 * Driver input pins:
 * 	PA8 (TIM1 CH1 for PWM on ice motor): PWMA/B
 * 	PA2: 								AI1/2
 * 	PA3: 								BI1/2
 * 	+5V: 								STBY
 * Driver output pins:
 * 	AO1/2 tied together
 * 	BO1/2 tied together
 *
 * TIM4: runs motor for set ms
 */

#include "stm32f103xb.h"
#include "motor.h"

#define OUTPUT_PUSH_PULL_PA2_PA3 		(0x33 << 8)
#define ALT_FUNC_PUSH_PULL_PA8			(0xB << 0)
#define PWM_MODE_1						(0x6 << 4)

#define PIN_IN1							(1 << 2)	//PA2
#define PIN_IN2							(1 << 3)	//PA3


void motorControlInit() {
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_TIM1EN);			//GPIOA and TIM1 RCC

	GPIOA->CRL &= ~(0xFF << GPIO_CRL_MODE2_Pos);
	GPIOA->CRL |= OUTPUT_PUSH_PULL_PA2_PA3;

	GPIOA->CRH &= ~(0xF);
	GPIOA->CRH |= ALT_FUNC_PUSH_PULL_PA8;

	TIM4->PSC = 7999;
	TIM4->ARR = 0xFFFF;
	TIM4->EGR |= TIM_EGR_UG;
	TIM4->SR &= ~(TIM_SR_UIF);

	//16KHz PWM
	TIM1->PSC = 0;
	TIM1->ARR = 499;													//8MHz / 500 = 16KHz

	TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M);									//Clear out channel 1
	TIM1->CCMR1 |= PWM_MODE_1 | TIM_CCMR1_OC1PE;

	TIM1->CCER |= TIM_CCER_CC1E_Msk;
	TIM1->BDTR |= TIM_BDTR_MOE_Msk;

	TIM1->CR1 |= TIM_CR1_CEN_Msk;

	GPIOA->BRR = (PIN_IN1 | PIN_IN2);
}

void setMotorSpeed(uint8_t pct) {
	if (pct > 100) pct = 100;

	uint16_t duty = ((uint32_t) pct * 499 + 50) / 100;
	TIM1->CCR1 = duty;

}

void motorOff() {
	GPIOA->BRR = (PIN_IN1 | PIN_IN2);
	TIM1->CCR1 = 0;
}

void motorRunMS(uint8_t dir, uint16_t ms) {
	switch (dir) {
	case 0:
		GPIOA->BSRR = PIN_IN1;
		GPIOA->BRR = PIN_IN2;
	break;
	case 1:
		GPIOA->BRR = PIN_IN1;
		GPIOA->BSRR = PIN_IN2;
	break;
	default:
		GPIOA->BRR = (PIN_IN1 | PIN_IN2);
	}

	TIM4->CR1 &= ~TIM_CR1_CEN;
	TIM4->CNT = 0;
	TIM4->ARR = ms;
	TIM4->SR &= ~TIM_SR_UIF;

	TIM4->CR1 |= TIM_CR1_CEN;
	while (!(TIM4->SR & TIM_SR_UIF));

	TIM4->CR1 &= ~(TIM_CR1_CEN);
	TIM4->SR &= ~TIM_SR_UIF;
	motorOff();
}

