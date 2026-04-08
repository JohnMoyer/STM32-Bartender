#include <stdint.h>
#include "stepper.h"
#include "delay.h"
#include "i2c.h"
#include "lcd.h"
#include "ledtest.h"
#include "button.h"
#include "motor.h"

#include "stm32f103xb.h"

//15mm diameter for drive gear

/*
 * Timers:
 * TIM2: Stepper motor timing steps
 * TIM3: Pump motor timer
 */

#define CCW 1
#define CW 0

extern volatile uint8_t stepper_done;
static volatile uint8_t btn8 = 0;
static volatile uint8_t btn9 = 0;
extern volatile uint32_t steps_remaining;
extern volatile uint32_t steps_total;

int main(void) {
    initLed();
    initI2C();
    lcd_init();
    stepperInit();
    initButtons();
    motor_control_init();

    i2c_clear_queue();
    lcd_clear_nb();
    lcd_set_cursor_nb(0, 0);
    lcd_print_nb("Red: 30cm right");
    lcd_set_cursor_nb(1, 0);
    lcd_print_nb("Blue: 30cm left");

    static uint32_t bar_counter = 0;
    static uint8_t moving = 0;
    static uint8_t pourStep = 0;

    for(;;) {
        i2c_process();

        if (btn8) {
        	pourStep = 1;
            btn8 = 0;
            moving = 1;
            i2c_clear_queue();
            moveMM(300, 1500, CW);
            lcd_clear_nb();
            lcd_set_cursor_nb(0, 0);
            lcd_print_nb("Moving right");
            //motor_on(0);
        }
        if (btn9) {
            btn9 = 0;
            moving = 1;
            i2c_clear_queue();
            moveMM(300, 1500, CCW);
            lcd_clear_nb();
            lcd_set_cursor_nb(0, 0);
            lcd_print_nb("Moving left");
            motor_on(0);
        }

        if (moving && !stepper_done && steps_remaining > 50) {
            bar_counter++;
            if (bar_counter >= 5000) {
                bar_counter = 0;
                lcd_progress_bar(steps_total - steps_remaining, steps_total);
            }
        }

        if (pourStep == 2 && active_motor == 0xFF) {
        	moveMM(300, 1500, CCW);
        	pourStep = 0;
        }

        if (stepper_done) {
            stepper_done = 0;
            moving = 0;
            if (pourStep == 1) {
            	pourStep = 2;
            	motor_run_ms(0, 5000);
            }


            EXTI->IMR |= EXTI_IMR_MR8 | EXTI_IMR_MR9;  // re-enable both
            i2c_clear_queue();
            lcd_clear_nb();
            lcd_set_cursor_nb(0, 0);
            lcd_print_nb("Red: 30cm right");
            lcd_set_cursor_nb(1, 0);
            lcd_print_nb("Blue: 30cm left");
        }
    }
}
/* B8 and B9 are the buttons to be used
 * B8: toggle through drink menu
 * B9: Start drink sequence
 * TIM3: debounce
 */
void EXTI9_5_IRQHandler(void) {

	if (EXTI->PR & EXTI_PR_PR8) {  								//PB8
		EXTI->PR = EXTI_PR_PR8;													//Clear flag
		if (!(GPIOB->IDR & (1 << EXTI_PR_PR8_Pos))) {							//Press occurred
			EXTI->IMR &= ~EXTI_IMR_MR8;											//Button off
			btn8 = 1;
		}
	}

	if (EXTI->PR & EXTI_PR_PR9) {
		EXTI->PR = EXTI_PR_PR9;													//Clear flag
		if (!(GPIOB->IDR & (1 << EXTI_PR_PR9_Pos))) {							//Press occurred
			EXTI->IMR &= ~EXTI_IMR_MR9;											//Button off
			btn9 = 1;
		}
	}
}

