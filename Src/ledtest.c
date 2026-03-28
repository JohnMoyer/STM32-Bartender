/*
 * ledtest.c
 *
 *  Created on: Feb 4, 2026
 *      Author: John Moyer
 */

#include <stdint.h>
#include "ledtest.h"
#include "stm32f103x6.h"


void initLed() {
    uint8_t rccMask = 0b1 << 4;
    RCC->APB2ENR |= rccMask;

    GPIOC->CRH &= ~(0b1111 << 20);  // Clear all 4 bits (CNF13[1:0] + MODE13[1:0])
    GPIOC->CRH |= (0b0010 << 20);   // Set MODE13=0b10 (2MHz), CNF13=0b00 (push-pull)

	GPIOC->ODR &= ~(0b1 << 13);
}

//PC13 is board led

void toggleLed() {
	GPIOC->ODR ^= 0b1 << 13;
}
