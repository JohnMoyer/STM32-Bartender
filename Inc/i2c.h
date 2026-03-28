/*
 * i2c.h
 *
 *  Created on: Feb 4, 2026
 *      Author: John Moyer
 */

#ifndef I2C_H_
#define I2C_H_

void initI2C(void);

void i2c_start(void);

int i2c_send_address(uint8_t address, uint8_t direction);

void i2c_write_data(uint8_t data);

void i2c_stop(void);


#define I2C_WRITE 0
#define I2C_READ 1

#endif /* I2C_H_ */
