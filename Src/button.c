/*
 * button.c
 *
 *  Created on: Mar 27, 2026
 *      Author: moyerjf
 */

#include "button.h"
#include "drinks.h"
#include "lcd.h"
#include "stm32f103xb.h"

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
	GPIOB->ODR |= 0b11 << GPIO_ODR_ODR8_Pos;			//Pull up resistors for B8 and B9

	AFIO->EXTICR[2] |= (0x01 << AFIO_EXTICR3_EXTI8_PB_Pos);
	AFIO->EXTICR[2] |= (0x01 << AFIO_EXTICR3_EXTI9_PB_Pos);

	EXTI->RTSR &= ~(0b11 << EXTI_RTSR_TR8_Pos);
	EXTI->FTSR |= (0b11 << EXTI_FTSR_TR8_Pos);
	EXTI->IMR  |= (0b11 << EXTI_IMR_MR8_Pos);

    TIM3->PSC = 8 - 1;
    TIM3->ARR = 20000 - 1;								//20ms period for TIM3
    TIM3->DIER |= TIM_DIER_UIE_Msk;
    TIM3->CR1 &= ~TIM_CR1_CEN_Msk;

	NVIC->ISER[0] |= ((1 << EXTI9_5_IRQn)
				  |   (1 << TIM3_IRQn));
}




