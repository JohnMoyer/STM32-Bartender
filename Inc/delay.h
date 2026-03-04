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

void delayStkMs(uint32_t ms);

void delayStkUs(uint32_t us);

void delayStkBusyMS(uint32_t N);


#endif /* DELAY_H_ */
