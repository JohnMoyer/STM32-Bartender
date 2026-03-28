/*
 * lcd.h
 *
 *  Created on: Feb 5, 2026
 *      Author: John Moyer
 */

#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>

void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print(const char* str);
void lcd_command(uint8_t cmd);
void lcd_data(uint8_t data);


#endif /* LCD_H_ */
