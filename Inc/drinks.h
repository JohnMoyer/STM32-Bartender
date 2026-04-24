/*
 * drinks.h
 *
 *  Created on: Mar 27, 2026
 *      Author: moyerjf
 */
#ifndef DRINKS_H
#define DRINKS_H

#include <stdint.h>

typedef struct {
    uint16_t position_mm;   // distance from home position
    uint8_t  motor_id;      // which pump motor to use
    uint32_t pour_ms;       // how long to pour in ms
} Ingredient;

typedef struct {
    const char* name;
    Ingredient  ingredients[4];  // max 4 ingredients
    uint8_t     num_ingredients;
} Drink;

extern const Drink drink_menu[];
extern const uint8_t num_drinks;

#endif
