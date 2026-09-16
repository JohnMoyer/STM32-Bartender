cmake_minimum_required(VERSION 3.22)

# The Arm GNU Toolchain's bin directory must be on PATH.
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(STM32_GNU_BIN
    "$ENV{CUBE_BUNDLE_PATH}/gnu-tools-for-stm32/14.3.1+st.2/bin"
)

find_program(ARM_GCC arm-none-eabi-gcc
    HINTS "${STM32_GNU_BIN}"
    REQUIRED
)
find_program(ARM_SIZE arm-none-eabi-size
    HINTS "${STM32_GNU_BIN}"
    REQUIRED
)
find_program(ARM_OBJCOPY arm-none-eabi-objcopy
    HINTS "${STM32_GNU_BIN}"
    REQUIRED
)

set(CMAKE_C_COMPILER "${ARM_GCC}")
set(CMAKE_ASM_COMPILER "${ARM_GCC}")

project(Bartender LANGUAGES C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(MCU_FLAGS -mcpu=cortex-m3 -mthumb)

add_executable(${PROJECT_NAME}
    Src/main.c
    Src/button.c
    Src/delay.c
    Src/drinks.c
    Src/i2c.c
    Src/lcd.c
    Src/ledtest.c
    Src/motor.c
    Src/selector.c
    Src/stepper.c
    Src/syscalls.c
    Src/sysmem.c
    Src/system_stm32f1xx.c
    Startup/startup_stm32f103c8tx.s
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

target_compile_definitions(${PROJECT_NAME} PRIVATE STM32F103xB)

target_compile_options(${PROJECT_NAME} PRIVATE
    ${MCU_FLAGS}
    -ffunction-sections
    -fdata-sections
    -Wall
    -Wextra
    $<$<CONFIG:Debug>:-Og>
    $<$<CONFIG:Debug>:-g3>
    $<$<CONFIG:Release>:-Os>
)

target_link_options(${PROJECT_NAME} PRIVATE
    ${MCU_FLAGS}
    -nostartfiles
    --specs=nano.specs
    --specs=nosys.specs
    -T${CMAKE_CURRENT_SOURCE_DIR}/STM32F103C8TX_FLASH.ld
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.map
)

target_link_libraries(${PROJECT_NAME} PRIVATE c m)
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${ARM_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
    COMMAND ${ARM_OBJCOPY} -O ihex
            $<TARGET_FILE:${PROJECT_NAME}>
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hex
    COMMAND ${ARM_OBJCOPY} -O binary
            $<TARGET_FILE:${PROJECT_NAME}>
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin
    COMMENT "Creating size report, HEX, and BIN files"
)

# Enables: cmake --build build --target flash
find_program(STM32_PROGRAMMER_CLI
    NAMES STM32_Programmer_CLI STM32_Programmer_CLI.exe
    HINTS "C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin"
)

if(STM32_PROGRAMMER_CLI)
    add_custom_target(flash
        COMMAND "${STM32_PROGRAMMER_CLI}"
                -c port=SWD
                -w $<TARGET_FILE:${PROJECT_NAME}>
                -v
                -rst
        DEPENDS ${PROJECT_NAME}
        USES_TERMINAL
        COMMENT "Flashing ${PROJECT_NAME}.elf through ST-Link"
    )
else()
    message(STATUS
        "STM32CubeProgrammer CLI not found; build works, but the flash target is unavailable."
    )
endif()

