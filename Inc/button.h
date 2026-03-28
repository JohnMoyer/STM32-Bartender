/*
 * button.h
 *
 *  Created on: Mar 27, 2026
 *      Author: moyerjf
 */

#ifndef BUTTON_H_
#define BUTTON_H_

/*
 * Function: initButtons
 * Params: none
 * Outputs: none
 * Description: enables portB, configures pins 3 to 7 to be inputs(00),
 * 					and enable pull-up resistors on pins 3 to 7.
 */
extern void initButtons();

extern void initButtonsIntr();


#endif /* BUTTON_H_ */
