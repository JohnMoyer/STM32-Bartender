/*
 * stepper.c
 *
 *  Created on: Mar 3, 2026
 *      Author: moyerjf
 */

#include "stm32f103x6.h"
#include "delay.h"

#define STEPS_PER_REVOLUTION 200

void stepperInit(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;  					// enable GPIOA clock

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
    delayStkUs(5);
    GPIOA->BSRR = GPIO_BSRR_BR1;   							// STEP low
    delayStkUs(5);
}

void spinDegrees(uint16_t degrees, uint32_t tMS, uint8_t dir) {
	if (dir) GPIOA->BSRR = GPIO_BSRR_BS0;
	else     GPIOA->BSRR = GPIO_BSRR_BR0;

	uint32_t steps = degrees * STEPS_PER_REVOLUTION / 360;

	uint32_t delayMs = (tMS) / steps;

	GPIOA->BSRR = GPIO_BSRR_BR2;                           // EN low (enable driver)
	delayStkUs(100);
	for (uint32_t i = 0; i < steps; i++) {
		step();
		delayStkMs(delayMs);
	}
	GPIOA->BSRR = GPIO_BSRR_BS2;                           // EN high (disable driver)
}

void spinTest(void) {
	GPIOA->BSRR = GPIO_BSRR_BR2;   							// EN low (enable driver)
	for (int i = 0; i < 200; i++) {							// 1 revolution at 200 steps/rev
		step();
		delayStkMs(5);										// speed control
	}
	delayStkMs(5000);										// pause between rotations
	GPIOA->BSRR = GPIO_BSRR_BS2;							// EN high (disable driver)
}

void spinContinuous(void) {
	GPIOA->BSRR = GPIO_BSRR_BR2;   							// EN low (enable driver)
	for(;;) {
		step();
		delayStkMs(3);										// speed control
	}														// EN high (disable driver)
}
