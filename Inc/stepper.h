 /*
 * stepper.h
 *
 *  Created on: Mar 3, 2026
 *      Author: moyerjf
 */

#ifndef STEPPER_H_
#define STEPPER_H_

extern volatile uint32_t steps_remaining;

extern volatile uint32_t steps_total;

extern void stepperInit(void);

extern void spinDegrees(uint16_t degrees, uint32_t tMS, uint8_t dir);

extern void moveMM(uint16_t MM, uint32_t tMS, uint8_t dir);

extern void spinTest(void);

extern void spinContinuous(void);

#endif /* STEPPER_H_ */
