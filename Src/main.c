#include <stdint.h>
#include "stepper.h"
#include "delay.h"
#include "i2c.h"
#include "lcd.h"
#include "ledtest.h"
#include "button.h"

#include "stm32f103x6.h"

//15mm diameter for drive gear

#define CCW 1
#define CW 0

int main(void) {
	initLed();
	initI2C();
	lcd_init();
    stepperInit();
    initButtons();
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Pouring Captain");
    lcd_set_cursor(1, 0);
    lcd_print("Coke...");
    for(;;) {
    	moveMM(100, 1500, CW);
    	delayStkMs(4000);
    	toggleLed();

    }

}
