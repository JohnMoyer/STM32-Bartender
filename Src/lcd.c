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

// PCF8574 pin mapping
#define LCD_BACKLIGHT 		0x08
#define LCD_EN 				0x04  // Enable bit
#define LCD_RW 				0x02  // Read/Write bit
#define LCD_RS 				0x01  // Register select bit

// Commands
#define LCD_CLEAR 			0x01
#define LCD_HOME 			0x02
#define LCD_ENTRY_MODE 		0x04
#define LCD_DISPLAY_CONTROL 0x08
#define LCD_FUNCTION_SET 	0x20

// Send 4 bits to LCD
static void lcd_send_nibble(uint8_t nibble, uint8_t mode) {
    uint8_t data = nibble | mode | LCD_BACKLIGHT;

    i2c_start();
    i2c_send_address(LCD_ADDR, I2C_WRITE);

    // Send with E high
    i2c_write_data(data | LCD_EN);
    delayStkMs(1);

    // Send with E low (falling edge latches data)
    i2c_write_data(data & ~LCD_EN);
    delayStkMs(1);

    i2c_stop();
}

// Send a byte (command or data)
static void lcd_send_byte(uint8_t byte, uint8_t mode) {
    uint8_t high_nibble = (byte & 0xF0);
    uint8_t low_nibble = (byte << 4) & 0xF0;

    lcd_send_nibble(high_nibble, mode);
    lcd_send_nibble(low_nibble, mode);
}

// Send command
void lcd_command(uint8_t cmd) {
    lcd_send_byte(cmd, 0);  // RS=0 for command
    delayStkMs(2);
}

// Send data (character)
void lcd_data(uint8_t data) {
    lcd_send_byte(data, LCD_RS);  // RS=1 for data
    delayStkMs(1);
}

// Initialize LCD
void lcd_init(void) {
    delayStkMs(50);  // Wait for LCD to power up

    // Initialize in 4-bit mode (special sequence)
    lcd_send_nibble(0x30, 0);
    delayStkMs(5);
    lcd_send_nibble(0x30, 0);
    delayStkMs(1);
    lcd_send_nibble(0x30, 0);
    delayStkMs(1);
    lcd_send_nibble(0x20, 0);  // Set to 4-bit mode
    delayStkMs(1);

    // Function set: 4-bit, 2 lines, 5x8 dots
    lcd_command(LCD_FUNCTION_SET | 0x08);

    // Display control: Display on, cursor off, blink off
    lcd_command(LCD_DISPLAY_CONTROL | 0x04);

    // Clear display
    lcd_command(LCD_CLEAR);
    delayStkMs(2);

    // Entry mode: Increment cursor, no shift
    lcd_command(LCD_ENTRY_MODE | 0x02);
}

// Clear screen
void lcd_clear(void) {
    lcd_command(LCD_CLEAR);
    delayStkMs(2);
}

// Set cursor position
void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t row_offsets[] = {0x00, 0x40};
    lcd_command(0x80 | (col + row_offsets[row]));
}

// Print string
void lcd_print(const char* str) {
    while(*str) {
        lcd_data(*str++);
    }
}
