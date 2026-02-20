/*
 * delay.c
 *
 *  Created on: Feb 4, 2026
 *      Author: John Moyer
 */

#include <stdint.h>
#include "delay.h"
#include "regaddr.h"


#define STK ((stk_t*) STK_BASE)

void delayBusyMS(uint32_t N) {
	for(uint32_t i=0; i<N*DELAY_MS; i++);
}

void delayStkBusyUS(uint32_t N) {
    uint32_t reload = (8 * N) - 1;
    if (reload > 0xFFFFFF) return;

    STK->CTRL = 0b101;
    STK->LOAD = reload;
    STK->VAL = 0;
    STK->CTRL = 0b101; //8MHz

    while(!(STK->CTRL & (1 << 16)));

}

void delayStkBusyMS(uint32_t N) {
    uint32_t reload = (8000 * N) - 1;
    if (reload > 0xFFFFFF) return;

    STK->CTRL = 0b101;
    STK->LOAD = reload;
    STK->VAL = 0;
    STK->CTRL = 0b101; //8MHz

    while(!(STK->CTRL & (1 << 16)));
}
