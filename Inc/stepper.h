/*
 * stepper.h
 *
 *  Created on: Mar 3, 2026
 *      Author: moyerjf
 */

#ifndef STEPPER_H_
#define STEPPER_H_

void stepperInit(void);

void spinDegrees(uint16_t degrees, uint32_t tMS, uint8_t dir);

void spinTest(void);

void spinContinuous(void);

#endif /* STEPPER_H_ */
