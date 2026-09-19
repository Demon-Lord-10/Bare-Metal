# STM32F401 Bare-Metal Firmware Notes

**Study notes for developing bare-metal firmware on the STM32F401CCU6 (WeAct BlackPill).**

---

## Introduction

This project is a from-scratch, bare-metal firmware environment for the **STM32F401CCU6** microcontroller — an ARM Cortex-M4 core running at up to 84 MHz, with 256 KB Flash and 64 KB SRAM.

**Bare-metal** means there is no operating system, no HAL library, and no vendor-generated startup code. Every byte that runs on the chip — from the vector table to the linker memory map — is written and understood by hand. The goal is to learn what actually happens between plugging in power and reaching `main()`.

### Why Bare-Metal?

| Approach | Pros | Cons |
|---|---|---|
| **Vendor HAL / CubeMX** | Fast prototyping, portable across STM32 families | Hides hardware details, large binary size |
| **CMSIS only** | Thin register-level access, no bloat | Still relies on vendor startup files |
| **Bare-metal (this project)** | Full understanding, minimal binary, total control | Slow to develop, must read datasheets carefully |

This project takes the third approach: we write our own **linker script**, **startup assembly**, and **peripheral drivers** using only the CMSIS device headers for register definitions.

---

## Target Hardware

| Parameter | Value |
|---|---|
| **MCU** | STM32F401CCU6 |
| **Core** | ARM Cortex-M4 (ARMv7E-M) with FPU |
| **Flash** | 256 KB (mapped at `0x0800_0000`) |
| **SRAM** | 64 KB (mapped at `0x2000_0000`) |
| **Max Clock** | 84 MHz |
| **Board** | WeAct MiniF4 "BlackPill" V3.1 |
| **Package** | UFQFPN48 |

---

## Table of Contents

| # | Chapter | What You'll Learn |
|---|---|---|
| 1 | [Memory Layout & Linker Script](01_memory_and_linker/linker_script.md) | How the linker maps `.text`, `.data`, `.bss` into Flash and RAM. LMA vs VMA. The `MEMORY` and `SECTIONS` commands. |
| 2 | [Startup Code & Boot Sequence](02_startup_and_boot/startup_assembly.md) | What happens at power-on: vector table, `reset_handler`, copying `.data`, zeroing `.bss`, and jumping to `main()`. |

---

## Reference Documents

The `Datasheet/` folder contains the primary references used throughout these notes:

| Document | Description |
|---|---|
| `STM32F401CCU6_Datasheet.pdf` | Pin-out, electrical characteristics, memory map |
| `rm0368-...pdf` | **Reference Manual** — peripheral register descriptions |
| `pm0214-...pdf` | **Programming Manual** — Cortex-M4 instruction set, NVIC, SysTick |
| `DUI0553.pdf` | ARM Cortex-M4 Technical Reference Manual |
| `MiniF4x1Cx_V31 SchDoc.pdf` | WeAct BlackPill V3.1 board schematic |

---

## Project Structure

```
.
├── linker.ld              # Linker script — memory layout & section placement
├── startup.s              # Startup assembly — vector table & reset handler
├── src/                   # Application source files (C)
├── inc/
│   └── CMSIS/             # CMSIS device headers (register definitions)
├── docs/                  # This documentation (MkDocs)
├── Datasheet/             # Reference PDFs
└── mkdocs.yml             # MkDocs configuration
```
