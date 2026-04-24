/*
 * i2c.c
 *
 *  Created on: Feb 4, 2026
 *      Author: John Moyer
 */

#include <stdint.h>
#include "i2c.h"
#include "delay.h"
#include "stm32f103xb.h"

#define I2C_QUEUE_SIZE 160

typedef struct {
    uint8_t addr;
    uint8_t data;
    uint16_t delay_us;
} I2CTransaction;

typedef enum {
    I2C_IDLE,
    I2C_START_SENT,
    I2C_ADDR_SENT,
    I2C_DATA_SENT
} I2C_State;

static volatile I2C_State i2c_state = I2C_IDLE;

static volatile I2CTransaction queue[I2C_QUEUE_SIZE];
static volatile uint16_t q_head = 0;
static volatile uint16_t q_tail = 0;
static volatile uint8_t reset_flag = 0;
static volatile uint8_t i2c_busy = 0;
static volatile I2CTransaction current;

void initI2C() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN_Msk;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN_Msk;

    GPIOB->CRL &= ~(0xFF << 24);
    GPIOB->CRL |= (0xFF << 24);

    I2C1->CR1 &= ~(1 << 0);
    I2C1->CR1 |= (1 << 15);
    for(volatile int i = 0; i < 1000; i++);
    I2C1->CR1 &= ~(1 << 15);

    I2C1->CR2 = 8;
    I2C1->CCR = 40;
    I2C1->TRISE = 9;

    I2C1->CR1 |= (1 << 0);
    NVIC_EnableIRQ(I2C1_EV_IRQn);
    NVIC_EnableIRQ(I2C1_ER_IRQn);
}

void i2c_queue_byte(uint8_t addr, uint8_t data, uint16_t delay_us) {
    uint8_t next = (q_tail + 1) % I2C_QUEUE_SIZE;
    if (next == q_head) return;
    queue[q_tail].addr = addr;
    queue[q_tail].data = data;
    queue[q_tail].delay_us = delay_us;
    q_tail = next;
}

void i2c_process(void) {
    if (i2c_busy) return;
    if (q_head == q_tail) return;

    current = queue[q_head];
    q_head = (q_head + 1) % I2C_QUEUE_SIZE;

    i2c_busy = 1;
    i2c_state = I2C_START_SENT;

    I2C1->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN;
    I2C1->CR1 |= I2C_CR1_START;
}

void i2c_clear_queue(void) {
    while (q_head != q_tail) {
    	i2c_process();
    }
}

// Blocking functions for lcd_init only
void i2c_start() {
    I2C1->CR1 |= (1 << 8);
    while (!(I2C1->SR1 & I2C_SR1_SB));
}

int i2c_send_address(uint8_t address, uint8_t direction) {
    I2C1->DR = (address << 1) | direction;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR1;
    (void)I2C1->SR2;	//A read of SR2 after a read of SR1 clears ADDR flag
    return 0;
}

void i2c_write_data(uint8_t data) {
    while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = data;
}

void i2c_stop() {
    while (!(I2C1->SR1 & I2C_SR1_BTF));
    I2C1->CR1 |= (1 << 9);
}

void I2C1_EV_IRQHandler(void) {
    uint32_t sr1 = I2C1->SR1;

    // START condition
    if ((sr1 & I2C_SR1_SB) && i2c_state == I2C_START_SENT) {
        I2C1->DR = (current.addr << 1);
        i2c_state = I2C_ADDR_SENT;
    }

    // Address sent
    else if ((sr1 & I2C_SR1_ADDR) && i2c_state == I2C_ADDR_SENT) {
        (void)I2C1->SR2;
        I2C1->DR = current.data;
        i2c_state = I2C_DATA_SENT;
    }

    // Byte transfer finished
    else if ((sr1 & I2C_SR1_BTF) && i2c_state == I2C_DATA_SENT) {
        I2C1->CR1 |= I2C_CR1_STOP;

        i2c_busy = 0;
        i2c_state = I2C_IDLE;

        I2C1->CR2 &= ~(I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN);
    }
}
void I2C1_ER_IRQHandler(void) {
    I2C1->SR1 = 0;
    I2C1->CR1 |= I2C_CR1_STOP;

    i2c_busy = 0;
    i2c_state = I2C_IDLE;
}
