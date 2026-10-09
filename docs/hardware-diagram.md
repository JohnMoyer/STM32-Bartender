# STM32 Bartender hardware block diagram

Open [stm32-bartender.drawio](stm32-bartender.drawio) in diagrams.net / draw.io using **File > Open From > Device**. The file is uncompressed XML, with editable blocks, labels and connectors on four pages:

1. **System overview:** MCU peripherals, user interface, stepper bank, DC motor driver, valve, debug and power.
2. **A4988 and motor selection:** conveyor and three recipe-referenced pump channels, selector address mapping, driver interfaces and fixed hardware connections.
3. **Clocks, timers and interrupts:** RCC, TIM1–TIM4, SysTick, I2C1, AFIO/EXTI, NVIC and application control flow.
4. **Pin inventory, power and open details:** every configured application pin, SWD pins, actual GPIO modes, supply domains and unresolved hardware details.


## Evidence and coverage

| Source | Diagram coverage |
| --- | --- |
| `Src/system_stm32f1xx.c` | Reset-default 8 MHz HSI clock configuration |
| `Src/main.c`, `Src/drinks.c` | State machine, station locations, recipe channels, ice/valve/purge durations |
| `Src/stepper.c`, `Src/selector.c` | TIM2/TIM3, STEP/DIR/enable GPIO, selector control and indices 0–7 |
| `Src/motor.c` | TIM1 PWM, TIM4 polling, DC direction pins and driver comments |
| `Src/i2c.c`, `Src/lcd.c` | I2C1, PB6/PB7, address 0x27, LCD backpack bit mapping and interrupts |
| `Src/button.c` | PB8/PB9, AFIO, EXTI8/9, NVIC and otherwise-unused PB5 output |
| `Src/valve.c`, `Inc/valve.h` | PA4 active-high valve control |
| `Src/ledtest.c`, `Src/delay.c` | PC13 board LED and polled SysTick delays |
| `README.md` | ST-Link, PA13/PA14, optional 3.3 V power and BOOT0 |
| Startup code, remaining application files and build source list | 


## Component references

- [ST RM0008 reference manual](https://www.st.com/resource/en/reference_manual/cd00171190-stm32f101-103-105-107-stm32f100-series-armbased-32bit-mcus-stmicroelectronics.pdf): clock tree, GPIO modes/reset state, timer and interrupt semantics.
- [Allegro A4988 datasheet](https://www.allegromicro.com/~/media/Files/Datasheets/A4988-Datasheet.pdf): STEP/DIR/enable, microstep/reset/sleep inputs, phase outputs and local supply/current-setting connections.
- [SparkFun TB6612FNG motor driver](https://www.sparkfun.com/sparkfun-motor-driver-dual-tb6612fng-1a.html): the board linked in `Src/motor.c`; this identifies the referenced product, not a verified installed board.

