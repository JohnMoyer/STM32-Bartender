/*
 * drinks.c
 *
 *  Created on: Apr 8, 2026
 *      Author: moyerjf
 */

#include "drinks.h"

const Drink drink_menu[] = {
    {
        .name = "Bottles 1 & 2",
        .num_ingredients = 2,
        .ingredients = {
            { .position_mm = 300, .motor_id = 1, .pour_ms = 1000 },
            { .position_mm = 600, .motor_id = 2, .pour_ms = 2500 },
        }
    },
    {
        .name = "Bottles 2 & 3",
        .num_ingredients = 2,
        .ingredients = {
            { .position_mm = 150, .motor_id = 2, .pour_ms = 1000 },
            { .position_mm = 250, .motor_id = 3, .pour_ms = 2500 },
        }
    },
};

const uint8_t num_drinks = sizeof(drink_menu) / sizeof(drink_menu[0]);
