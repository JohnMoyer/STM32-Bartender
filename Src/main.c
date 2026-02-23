#include <stdint.h>
#include "motor.h"
#include "delay.h"

int main(void)
{

	motor_control_init();
    /* Loop forever */
	for(;;) {
		delayBusyMS(3000);
		motor_on();
		delayBusyMS(3000);
		motor_off();
	}
}
