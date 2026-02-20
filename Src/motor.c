/*
 * motor.c
 *
 *  Created on: Feb 20, 2026
 *      Author: moyerjf
 */


#include "regaddr.h"
#include "motor.h"

#define GPIOA ((gpio_t*) GPIOA_BASE)

void motor_control_init() {
    volatile uint32_t* rcc_apb2enr = (uint32_t*) RCC_APB2ENR;
    *rcc_apb2enr |= (0b1 << 2);

    //PA0 for motor
    GPIOA->CRL &= ~(0xF << 0);
    GPIOA->CRL |= (0b0010 << 0);

    motor_off();

}

void motor_on() {
	GPIOA->ODR |= (0b1 << 0);
}

void motor_off() {
	GPIOA->ODR &= ~(0b1 << 0);
}
