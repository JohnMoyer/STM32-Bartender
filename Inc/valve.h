#ifndef VALVE_H
#define VALVE_H

#include <stdint.h>

#define PIN_VALVE_1					    (1 << 4)	    //PA4
#define OUTPUT_PUSH_PULL_PA4            (0x3 << 16)

extern void valveInit();

extern void valveOn(uint8_t valve);

extern void valveOff(uint8_t valve);

#endif /* VALVE_H_ */