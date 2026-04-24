/*
 * selector.c
 *
 *  Created on: Apr 22, 2026
 *      Author: moyerjf
 */


#include "stm32f103xb.h"
#include "selector.h"

#define LSB 13


/*
 * Pins:
 * PB12: enable; active high
 * PB13-15: binary encoded to motor index. LSB: PB13
 */
void initSelector() {
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN_Msk;

	GPIOB->CRH |= (0x2222 << GPIO_CRH_MODE12_Pos);
	GPIOB->BRR = GPIO_BRR_BR12_Msk;					//enable off (active high)
	GPIOB->BRR = 0x8 << GPIO_BRR_BR13_Pos;			//Selectors all low (selecting 0)
}


void enableSelector() {
	GPIOB->BSRR = GPIO_BSRR_BS12_Msk;
}

void disableSelector() {
	GPIOB->BRR = GPIO_BRR_BR12_Msk;
}

void selectStepper(uint8_t index) {
	GPIOB->BRR = (0x7 << LSB);
	GPIOB->BSRR = (index & 0x7) << LSB;
}
