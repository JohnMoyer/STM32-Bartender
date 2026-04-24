/*
 * selector.h
 *
 *  Created on: Apr 22, 2026
 *      Author: moyerjf
 */

#ifndef SELECTOR_H_
#define SELECTOR_H_

#include "stm32f103xb.h"
#include <stdint.h>

extern void initSelector();

extern void enableSelector();

extern void disableSelector();

extern void selectStepper(uint8_t index);

#endif /* SELECTOR_H_ */
