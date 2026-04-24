#include <stdint.h>
#include "stepper.h"
#include "delay.h"
#include "i2c.h"
#include "lcd.h"
#include "ledtest.h"
#include "button.h"
#include "motor.h"
#include "drinks.h"
#include "stm32f103xb.h"
#define CCW 1
#define CW  0
#define ONE_SECOND 1000
#define ONE_AND_HALF_SECOND 1500
#define TWO_SECOND 2000
#define THREE_SECOND 3000

extern volatile uint8_t  stepper_done;
extern volatile uint32_t steps_remaining;
extern volatile uint32_t steps_total;
extern volatile uint8_t active_motor;


static volatile uint8_t btn8 = 0;
static volatile uint8_t btn9 = 0;

typedef enum {
    STATE_IDLE,
    STATE_POURING,
    STATE_MOVING_TO_NEXT,
    STATE_RETURNING_HOME
} MachineState;

static MachineState state         = STATE_IDLE;
static uint8_t      selected_drink = 0;
static uint8_t      current_ingredient = 0;
static uint32_t     bar_counter   = 0;
static uint16_t     current_pos_mm = 0;  // track position from home

static void update_progress_bar(void) {
    bar_counter++;
    if (bar_counter >= 5000) {
        bar_counter = 0;
        lcd_progress_bar(steps_total - steps_remaining, steps_total);
    }
}

static void show_idle_screen(void) {
    i2c_clear_queue();
    lcd_clear_nb();
    lcd_set_cursor_nb(0, 0);
    lcd_print_nb(drink_menu[selected_drink].name);
    lcd_set_cursor_nb(1, 0);
    lcd_print_nb("Pour      Next>>");
}

int main(void) {
    initLed();
    initI2C();
    lcd_init();
    stepperInit();
    initButtons();
    motor_control_init();


    show_idle_screen();

    for(;;) {
        i2c_process();

        // btn8 = cycle through drinks
        if (btn8 && state == STATE_IDLE) {
            btn8 = 0;
            selected_drink = (selected_drink + 1) % num_drinks;
            show_idle_screen();
            EXTI->PR  = EXTI_PR_PR8;
            EXTI->IMR |= EXTI_IMR_MR8;
        }

        // btn9 = start pouring sequence
        if (btn9 && state == STATE_IDLE) {
            btn9 = 0;
            current_ingredient = 0;
            state = STATE_MOVING_TO_NEXT;

            const Ingredient* ing = &drink_menu[selected_drink].ingredients[0];
            uint16_t dist = ing->position_mm - current_pos_mm;
            uint8_t  dir  = (ing->position_mm > current_pos_mm) ? CW : CCW;

            i2c_clear_queue();
            lcd_clear_nb();
            lcd_set_cursor_nb(0, 0);
            lcd_print_nb("Moving...");
            bar_counter = 0;

            moveMM(dist, ONE_AND_HALF_SECOND, dir);

        }

        // Update progress bar while moving
        if (state == STATE_MOVING_TO_NEXT || state == STATE_RETURNING_HOME) {
            if (!stepper_done && steps_remaining > 50) {
                update_progress_bar();
            }
        }

        if (state == STATE_POURING) {
            if (stepper_done) {
            	stepper_done = 0;
                // Pour finished
                const Drink* drink = &drink_menu[selected_drink];
                current_ingredient++;

                if (current_ingredient < drink->num_ingredients) {
                    // Move to next ingredient
                    state = STATE_MOVING_TO_NEXT;
                    const Ingredient* ing = &drink->ingredients[current_ingredient];
                    uint16_t dist = (ing->position_mm > current_pos_mm) ?
                                    ing->position_mm - current_pos_mm :
                                    current_pos_mm - ing->position_mm;
                    uint8_t dir = (ing->position_mm > current_pos_mm) ? CW : CCW;

                    i2c_clear_queue();
                    lcd_clear_nb();
                    lcd_set_cursor_nb(0, 0);
                    lcd_print_nb("Moving...");
                    bar_counter = 0;
                    moveMM(dist, ONE_AND_HALF_SECOND, dir);
                } else {
                    // All ingredients done - return home
                    state = STATE_RETURNING_HOME;
                    i2c_clear_queue();
                    lcd_clear_nb();
                    lcd_set_cursor_nb(0, 0);
                    lcd_print_nb("Returning...");
                    bar_counter = 0;
                    moveMM(current_pos_mm, TWO_SECOND, CCW);
                }
            }
        }

        if (stepper_done && state != STATE_IDLE && state != STATE_POURING) {
            stepper_done = 0;

            const Ingredient* ing = &drink_menu[selected_drink].ingredients[current_ingredient];

            if (state == STATE_RETURNING_HOME) {
                current_pos_mm = 0;
                state = STATE_IDLE;
                show_idle_screen();
            } else {
                // Arrived at ingredient - update position and start pouring
                current_pos_mm = ing->position_mm;
                state = STATE_POURING;

                i2c_clear_queue();
                lcd_clear_nb();
                lcd_set_cursor_nb(0, 0);
                lcd_print_nb("Pouring...");

                pourDrink(ing->motor_id, ing->pour_ms);
            }
        }
    }
}

void EXTI9_5_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR8) {
        EXTI->PR = EXTI_PR_PR8;
        if (!(GPIOB->IDR & (1 << EXTI_PR_PR8_Pos))) {
            EXTI->IMR &= ~EXTI_IMR_MR8;
            btn8 = 1;
        }
    }
    if (EXTI->PR & EXTI_PR_PR9) {
        EXTI->PR = EXTI_PR_PR9;
        if (!(GPIOB->IDR & (1 << EXTI_PR_PR9_Pos))) {
            EXTI->IMR &= ~EXTI_IMR_MR9;
            btn9 = 1;
        }
    }
}

