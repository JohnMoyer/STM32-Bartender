# STM32 Bartender Project
Programming, electronics: **John Moyer**
- moyerjf@msoe.edu

Physical designing: **Isaac Wolf, Talan Luedtke**
- wolfi@msoe.edu
- luedtket@msoe.edu

## Purpose

TODO

## Build and run with VS Code and ST-Link

This is a bare-metal STM32F103C8 (Blue Pill) project. It does not use STM32
HAL or CubeMX-generated code.

### Requirements

- Visual Studio Code
- The **STM32CubeIDE for Visual Studio Code** extension pack
- The GNU Tools for STM32, CMake, Ninja, ST-Link GDB server, and programmer
  bundles installed through the extension's Bundle Manager
- An ST-Link connected to the Blue Pill

### ST-Link wiring

| ST-Link | Blue Pill |
| --- | --- |
| SWDIO | PA13 / SWDIO |
| SWCLK | PA14 / SWCLK |
| GND | GND |
| 3.3 V | 3.3 V, only if the ST-Link powers the board |

Set the Blue Pill's `BOOT0` jumper to `0`.

### Configure the project

Open the repository root in VS Code, open a PowerShell terminal, and run this
once (and again after deleting the build directory):

```powershell
cube-cmake --preset debug
```

The generated build files are placed in `build/debug`.

### Build

The Build button supplied by the STM32 extension currently does not work for
this manually configured project. Build from the VS Code terminal instead:

```powershell
cube-cmake --build --preset debug
```

The main debug image is generated at:

```text
build/debug/Bartender.elf
```

The build also generates `Bartender.hex`, `Bartender.bin`, and
`Bartender.map` in the same directory.

### Flash and debug

Build the project using the command above, then:

1. Connect and power the Blue Pill through the ST-Link.
2. Open **Run and Debug** in VS Code.
3. Select **STM32Cube: Launch ST-Link GDB Server**.
4. Press `F5`.
5. The debugger programs `build/debug/Bartender.elf` and stops at `main()`.
6. Press `F5` again to continue running the firmware.

The launch configuration intentionally references the ELF directly and does
not use the extension's failing automatic pre-build action. Re-run the build
command before pressing `F5` whenever source code changes.

### Command-line flash without debugging

To build, program, verify, reset, and run the firmware without starting a
debug session:

```powershell
cube-cmake --build --preset debug --target flash
```

If VS Code does not recognize the CMake project after opening it, run
**CMake: Select Configure Preset**, choose **STM32 Debug**, and then run
**CMake: Configure** from the Command Palette.
