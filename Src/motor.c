/*
 * motor.c
 *
 *  Created on: Feb 20, 2026
 *      Author: moyerjf
 */


#include "stm32f103x6.h"
#include "motor.h"

void motor_control_init() {
    RCC->APB2ENR |= (0b1 << 2);

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
