/*
 * delay.h
 *
 *  Created on: Feb 11, 2026
 *      Author: John Moyer
 */

#ifndef DELAY_H_
#define DELAY_H_
#include <stdint.h>

#define DELAY_MS 555

//72MHz default clock speed

void delayBusyMS(uint32_t N);

void delayStkBusyUS(uint32_t N);

void delayStkBusyMS(uint32_t N);


#endif /* DELAY_H_ */
