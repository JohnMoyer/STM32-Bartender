/*
 * drinks.h
 *
 *  Created on: Mar 27, 2026
 *      Author: moyerjf
 */

#ifndef DRINKS_H_
#define DRINKS_H_

#include <stdint.h>

typedef struct {
	uint32_t pos1_mm;
	uint32_t pour1_ms;
	uint32_t pos2_mm;
	uint32_t pour2_ms;
} drink_t;

#endif /* DRINKS_H_ */
