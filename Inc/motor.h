/*
 * motor.h
 *
 *  Created on: Feb 20, 2026
 *      Author: moyerjf
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>

extern volatile uint8_t active_motor;

extern void motor_control_init();

extern void motor_on(uint8_t motorIndex);

extern void motor_off(uint8_t motorIndex);

extern void motor_run_ms(uint8_t motorIndex, uint32_t ms);


#endif /* MOTOR_H_ */
