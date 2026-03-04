/*
 * delay.c
 *
 *  Created on: Feb 4, 2026
 *      Author: John Moyer
 */

#include <stdint.h>
#include "delay.h"
#include "stm32f103x6.h"


void delayStkUs(uint32_t us) {
    SysTick->LOAD = 8 - 1;
    SysTick->VAL  = 0;
    SysTick->CTRL = 5;
    for (uint32_t i = 0; i < us; i++) {
        while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    }
    SysTick->CTRL = 0;
}


void delayStkMs(uint32_t ms) {
    SysTick->LOAD = 8000 - 1;
    SysTick->VAL  = 0;
    SysTick->CTRL = 5;
    for (uint32_t i = 0; i < ms; i++) {
        while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    }
    SysTick->CTRL = 0;
}
