/*
 * button.c
 *
 *  Created on: Mar 27, 2026
 *      Author: moyerjf
 */

#include "button.h"
#include "drinks.h"
#include "stm32f103x6.h"

/*
 * Timers: TIM3 for debounce
 */

void initButtons() {
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN_Msk;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN_Msk;
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN_Msk;

    GPIOB->CRL &= ~(GPIO_CRL_CNF5 | GPIO_CRL_MODE5);
    GPIOB->CRL |= GPIO_CRL_MODE5_0 | GPIO_CRL_MODE5_1;


	GPIOB->CRH &= ~(0xFF);								//Clear 8 and 9, MODE00 is input so keep
	GPIOB->CRH |= (GPIO_CRH_CNF9_1 | GPIO_CRH_CNF8_1);	//CNF1 (0b10) is input with pull up/down

	GPIOB->ODR |= 0b11 << GPIO_ODR_ODR8_Pos;			//Pull up resistors

	EXTI->FTSR |= (0b11 << EXTI_FTSR_TR8_Pos);
	EXTI->IMR  |= (0b11 << EXTI_IMR_MR8_Pos);

    TIM3->PSC = 8 - 1;
    TIM3->ARR = 1000 - 1;
    TIM3->DIER |= TIM_DIER_UIE_Msk;
    TIM3->CR1 |= TIM_CR1_CEN_Msk;

	NVIC->ISER[0] |= ((1 << EXTI9_5_IRQn)
				  |   (1 << TIM3_IRQn));
}


void TIM3_IRQHandler(void) {
	if (TIM3->SR & TIM_SR_UIF) {
		TIM3->SR &= ~TIM_SR_UIF;
		GPIOB->ODR ^= GPIO_ODR_ODR5;
	}
}

void EXTI9_5_IRQHandler(void) {
	uint32_t pending = (EXTI->PR & (0b11 << EXTI_PR_PR8_Pos)) >> EXTI_PR_PR8_Pos;

	if (pending == 8) {  								//PB8
		EXTI->PR = EXTI_PR_PR8;													//Clear flag
		if (!(GPIOB->IDR & (1 << EXTI_PR_PR8_Pos))) {							//Press occurred
			EXTI->IMR &= ~EXTI_IMR_MR8;											//Button off
			//toggleLed(4);

			TIM3->CNT = 0;
			TIM3->SR &= ~TIM_SR_UIF;
			TIM3->CR1 |= TIM_CR1_CEN;											//TIM4 IRQ will re-enable button

		}
	} else if (pending == 9) {
		EXTI->PR = EXTI_PR_PR9;													//Clear flag
		if (!(GPIOB->IDR & (1 << EXTI_PR_PR9_Pos))) {							//Press occurred
			EXTI->IMR &= ~EXTI_IMR_MR9;											//Button off

			//toggleLed(4);
			TIM3->CNT = 0;
			TIM3->SR &= ~TIM_SR_UIF;
			TIM3->CR1 |= TIM_CR1_CEN;											//TIM3 IRQ will re-enable button

				}
	}
}

