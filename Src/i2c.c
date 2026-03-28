/*
 * i2c.c
 *
 *  Created on: Feb 4, 2026
 *      Author: John Moyer
 */

#include <stdint.h>
#include "i2c.h"
#include "delay.h"
#include "ledtest.h"
#include "stm32f103x6.h"

void initI2C() {

    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN_Msk;   // Enable GPIOB
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN_Msk;  // Enable I2C1

    // Configure PB6/PB7 AF open-drain 50MHz
    GPIOB->CRL &= ~(0xFF << 24);
    GPIOB->CRL |= (0xFF << 24);

    // Disable I2C before config
    I2C1->CR1 &= ~(1 << 0);

    // Software reset
    I2C1->CR1 |= (1 << 15);
    for(volatile int i = 0; i < 1000; i++);
    I2C1->CR1 &= ~(1 << 15);

    I2C1->CR2 = 8;        // 8MHz peripheral clock
    I2C1->CCR = 40;       // 8MHz / (2 * 100kHz) = 40
    I2C1->TRISE = 9;      // (8MHz * 1000ns / 1e9) + 1 = 9

//    I2C1->CR2 = 36;        // 8MHz peripheral clock
//    I2C1->CCR = 180;       // 8MHz / (2 * 100kHz) = 40
//    I2C1->TRISE = 37;      // (8MHz * 1000ns / 1e9) + 1 = 9

    // Enable I2C
    I2C1->CR1 |= (1 << 0);
}

static int i2c_wait_flag_timeout(volatile uint32_t* reg, uint32_t flag, uint32_t timeout) {
    while(!((*reg) & flag)) {
        if(timeout-- == 0) {
            toggleLed();  // Blink LED on timeout
            delayStkMs(50);
            return -1;  // Timeout
        }
    }
    return 0;  // Success
}

void i2c_start() {

    //Start bit
    I2C1->CR1 |= (1 << 8);
    //Wait for start bit
    i2c_wait_flag_timeout(&I2C1->SR1, (1 << 0), 100000);
}

//Param address: i2c device address (0x27 for lcd)
//Param direction: 0: w, 1: r
int i2c_send_address(uint8_t address, uint8_t direction) {

    //Data register set to address, set bit 0 to direction
    I2C1->DR = (address << 1) | direction;

    int result = i2c_wait_flag_timeout(&I2C1->SR1, (0b1 << 1), 100000);
    if(result < 0) return -1;  // Timeout or NACK

    // Clear ADDR
    (void)I2C1->SR1;
    (void)I2C1->SR2;

    // Check for Acknowledgment failure
    if(I2C1->SR1 & (1 << 10)) return -1;  // AF bit set = NACK

    return 0;  // Success
}

void i2c_write_data(uint8_t data) {
//    volatile uint32_t* i2c_dr = (uint32_t*) I2C_DR;
//    volatile uint32_t* i2c_sr1 = (uint32_t*) I2C_SR1;

    i2c_wait_flag_timeout(&I2C1->SR1, (1 << 7), 100000);  // Wait for TxE
    I2C1->DR = data;
}

void i2c_stop() {
    i2c_wait_flag_timeout(&I2C1->SR1, (1 << 2), 100000);  // Wait for BTF
    I2C1->CR1 |= (1 << 9);  // STOP
}
