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
void lcd_data(uint8_t data);
void lcd_print_nb(const char* str);
void lcd_clear_nb(void);
void i2c_process(void);
void lcd_set_cursor_nb(uint8_t row, uint8_t col);
void lcd_progress_bar(uint32_t current, uint32_t total);

#endif /* LCD_H_ */
