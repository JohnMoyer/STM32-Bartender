/*
 * delay.c
 *
 *  Created on: Feb 4, 2026
 *      Author: John Moyer
 */

#include <stdint.h>
#include "delay.h"
#include "stm32f103x6.h"

#define CPU_FREQ 8000000UL

void delayStkUs(uint32_t us) {
    SysTick->LOAD = us - 1;
    SysTick->VAL  = 0;
    SysTick->CTRL = 0x1;

    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));

    SysTick->CTRL = 0;
}

void delayStkMs(uint32_t ms) {
    SysTick->LOAD = (ms * 1000) - 1;
    SysTick->VAL  = 0;
    SysTick->CTRL = 0x1;

    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));


    SysTick->CTRL = 0;
}
