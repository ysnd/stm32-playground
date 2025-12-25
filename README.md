# STM32 Playground - Learning by Doing

This repo is my artifact of learning STM32, from HAL-based experiments to baremetal control,
focused on understanding how STM32 works.

## Core Idea
- STM32CubeMX is used once
- All generated files are stored in `bsp/stm32f103c8/` (Board Support Package)
- Each experiment lives in its own folder at the repository root
- Each experiment use their own `main.c` and adjust `Makefile` to link it to same bsp
This approach allows experimenting with different concepts without regenerating code or changing baseline since only one board is used and for clean/consistent repo.

## Setup Used

| Component | Details |
|---|---|
| **MCU** | STM32F103C8T6 - 72MHz, 64KB flash, Cortex-M3 |
| **Toolchain** | `arm-none-eabi-gcc`, `openocd`, `make` |
| **Hardware** | ST-Link V2, and some sensor i have |
| **Editor** | Neovim |
| **OS** | Linux |

## Goals
- Understanding clocks, GPIO, Timers, interrupts, and memory mapping in STM32
- Compare HAL vs direct register access
- Learn debbuging using GDB
