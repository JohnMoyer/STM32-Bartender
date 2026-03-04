#include <stdint.h>
#include "stepper.h"
#include "delay.h"

#include "stm32f103x6.h"

#define CCW 1
#define CW 0

int main(void) {
    stepperInit();
    for(;;) {
    	spinDegrees(360, 1000, CW);
    	delayStkMs(2000);
    }
}
