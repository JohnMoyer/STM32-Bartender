/*
 * drinks.c
 *
 *  Created on: Apr 8, 2026
 *      Author: moyerjf
 */

#include "drinks.h"

const Drink drink_menu[] = {
    {
        .name = "Tasty One",
        .num_ingredients = 1,
        .includes_valve_1 = 1,
        .ingredients = {
            { .position_mm = 440, .motor_id = 1, .pour_ms = 10000 },
        }
    },
    {
        .name = "Drink 2",
        .num_ingredients = 2,
        .includes_valve_1 = 0,
        .ingredients = {
            { .position_mm = 150, .motor_id = 2, .pour_ms = 1000 },
            { .position_mm = 250, .motor_id = 3, .pour_ms = 2500 },
        }
    },
    {
        .name = "Valve Only",
        .num_ingredients = 0,
        .includes_valve_1 = 1,
        .ingredients = {
            { .position_mm = 0, .motor_id = 0, .pour_ms = 0 },
            { .position_mm = 0, .motor_id = 0, .pour_ms = 0 },
        }
    }
};

const uint8_t num_drinks = sizeof(drink_menu) / sizeof(drink_menu[0]);
