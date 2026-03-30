/*
 * lcd.c
 *
 *  Created on: Feb 4, 2026
 *      Author: John Moyer
 */

#include <stdint.h>
#include "i2c.h"
#include "lcd.h"
#include "delay.h"

#define LCD_ADDR 0x27

#define LCD_BACKLIGHT       0x08
#define LCD_EN              0x04
#define LCD_RW              0x02
#define LCD_RS              0x01

#define LCD_CLEAR           0x01
#define LCD_HOME            0x02
#define LCD_ENTRY_MODE      0x04
#define LCD_DISPLAY_CONTROL 0x08
#define LCD_FUNCTION_SET    0x20

// Non-blocking queue functions
static void lcd_queue_nibble(uint8_t nibble, uint8_t mode) {
    uint8_t data = nibble | mode | LCD_BACKLIGHT;
    i2c_queue_byte(LCD_ADDR, data | LCD_EN, 0);
    i2c_queue_byte(LCD_ADDR, data & ~LCD_EN, 50);
}

static void lcd_queue_byte_nb(uint8_t byte, uint8_t mode) {
    lcd_queue_nibble(byte & 0xF0, mode);
    lcd_queue_nibble((byte << 4) & 0xF0, mode);
}

void lcd_progress_bar(uint32_t current, uint32_t total) {
    // 16 block characters = full bar
	if (total == 0) return;
    uint8_t filled = (current * 16) / total;

    lcd_set_cursor_nb(1, 0);
    for (uint8_t i = 0; i < 16; i++) {
        if (i < filled) {
            lcd_queue_byte_nb(0xFF, LCD_RS);  // full block character
        } else {
            lcd_queue_byte_nb(' ', LCD_RS);   // empty
        }
    }
}

void lcd_print_nb(const char* str) {
    while (*str) {
        lcd_queue_byte_nb(*str++, LCD_RS);
    }
}

void lcd_clear_nb(void) {
    lcd_queue_byte_nb(LCD_CLEAR, 0);
    i2c_queue_byte(LCD_ADDR, LCD_BACKLIGHT, 2000);
}

void lcd_set_cursor_nb(uint8_t row, uint8_t col) {
    uint8_t row_offsets[] = {0x00, 0x40};
    lcd_queue_byte_nb(0x80 | (col + row_offsets[row]), 0);
    i2c_queue_byte(LCD_ADDR, LCD_BACKLIGHT, 500);
}

// Blocking functions for lcd_init only
static void lcd_send_nibble(uint8_t nibble, uint8_t mode) {
    uint8_t data = nibble | mode | LCD_BACKLIGHT;
    i2c_start();
    i2c_send_address(LCD_ADDR, I2C_WRITE);
    i2c_write_data(data | LCD_EN);
    delayStkMs(1);
    i2c_write_data(data & ~LCD_EN);
    delayStkMs(1);
    i2c_stop();
}

static void lcd_send_byte(uint8_t byte, uint8_t mode) {
    lcd_send_nibble(byte & 0xF0, mode);
    lcd_send_nibble((byte << 4) & 0xF0, mode);
}

static void lcd_command(uint8_t cmd) {
    lcd_send_byte(cmd, 0);
    delayStkMs(2);
}

void lcd_init(void) {
    delayStkMs(50);
    lcd_send_nibble(0x30, 0);
    delayStkMs(5);
    lcd_send_nibble(0x30, 0);
    delayStkMs(1);
    lcd_send_nibble(0x30, 0);
    delayStkMs(1);
    lcd_send_nibble(0x20, 0);
    delayStkMs(1);
    lcd_command(LCD_FUNCTION_SET | 0x08);
    lcd_command(LCD_DISPLAY_CONTROL | 0x04);
    lcd_command(LCD_CLEAR);
    delayStkMs(2);
    lcd_command(LCD_ENTRY_MODE | 0x02);
}
