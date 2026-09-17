#include "stm32f103xb.h"
#include "valve.h"
#include <stdint.h>

void valveInit() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;     // Enable GPIOA clock

    // Configure PA4 as output push-pull
    GPIOA->CRL &= ~(0xF << GPIO_CRL_MODE4_Pos);        
    GPIOA->CRL |= OUTPUT_PUSH_PULL_PA4;

    GPIOA->BRR = PIN_VALVE_1;               // Valve 1 off
}

void valveOn(uint8_t valve) {
    switch (valve) {
        case 1:
            GPIOA->BSRR = PIN_VALVE_1;
            break;
        default:
            break;
    }
}

void valveOff(uint8_t valve) {
    switch (valve) {
        case 1:
            GPIOA->BRR = PIN_VALVE_1;
            break;
        default:
            break;
    }
}