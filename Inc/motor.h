/*
 * motor.h
 *
 *  Created on: Feb 20, 2026
 *      Author: moyerjf
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>

extern void motorControlInit();

extern void motorOff();

extern void motorRunMS(uint8_t dir, uint16_t ms);

extern void setMotorSpeed(uint8_t pct);


#endif /* MOTOR_H_ */
