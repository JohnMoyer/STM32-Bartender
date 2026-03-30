/*
 * i2c.h
 *
 *  Created on: Feb 4, 2026
 *      Author: John Moyer
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

extern void initI2C(void);

extern void i2c_start(void);

extern int i2c_send_address(uint8_t address, uint8_t direction);

extern void i2c_write_data(uint8_t data);

extern void i2c_stop(void);

extern void i2c_queue_byte(uint8_t addr, uint8_t data, uint16_t delay_us);

//extern uint8_t i2c_busy(void);

void i2c_wait(void);

extern void i2c_process(void);

void i2c_drain(void);

void i2c_clear_queue(void);

#define I2C_WRITE 0
#define I2C_READ 1

#endif /* I2C_H_ */
