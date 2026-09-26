# Bare Metal using STM32

Hi, I am trying to write bare-metal code for STM32 at the end of which I am hoping to understand how peripherals work and how different protocols work. This project makes use of an open-source toolchain like GCC, Makefile, and OpenOCD.

📖 **Documentation**: [https://demon-lord-10.github.io/Bare-Metal/](https://demon-lord-10.github.io/Bare-Metal/)

## Intention
The main motivation behind this project is to step away from vendor HAL libraries and auto-generated CubeMX code to understand what actually happens at the hardware level.
Instead of treating the microcontroller as a black box:
- Understand every byte executed between plugging in power and reaching `main()`.
- Learn how to interact with hardware peripherals directly through memory-mapped registers.
- Learn how to read datasheets and reference manuals to write drivers from scratch.
---

## 🛠️ Hardware & Tools
- **MCU**: STM32F401CCU6 (ARM Cortex-M4 @ 84 MHz)
- **Board**: WeAct BlackPill V3.1
- **Debugger**: ST-Link V2
- **Toolchain**: `arm-none-eabi-gcc`, GNU Make, OpenOCD

All datasheets are present in the datasheet section.

---

## 📁 Project Structure
```
.
├── inc/         # CMSIS and peripheral driver headers (rcc.h, gpio.h)
├── src/         # Driver implementations & main application
├── docs/        # Detailed study notes & documentation source
├── Datasheet/   # All used datasheets
├── linker.ld    # Custom GNU linker script
├── startup.s    # Startup assembly & vector table
└── Makefile     # Build and flash automation
```
## ⚡ Build & Flash
```bash
# Compile firmware (.elf and .bin)
make
# Flash to BlackPill via ST-Link & OpenOCD
make flash
# Clean build artifacts
make clean
```
