/*
 * regaddr.h
 *
 *  Created on: Feb 11, 2026
 *      Author: John Moyer
 */

#ifndef REGADDR_H_
#define REGADDR_H_

#include <stdint.h>

#define GPIOA_BASE  0x40010800
#define GPIOB_BASE 	0x40010C00
#define GPIOC_BASE	0x40011000

#define RCC_BASE	0x40021000

#define STK_BASE 	0xE000E010

#define RCC_APB2ENR 0x40021018

#define LCD_BACKLIGHT 0x08
#define LCD_EN        0x04
#define LCD_RS        0x01

typedef struct {
	volatile uint32_t CTRL;
	volatile uint32_t LOAD;
	volatile uint32_t VAL;
	volatile uint32_t CALIB;
}stk_t;

typedef struct {
	volatile uint32_t CRL;
	volatile uint32_t CRH;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t BRR;
	volatile uint32_t LCKR;
}gpio_t;

#endif /* REGADDR_H_ */
