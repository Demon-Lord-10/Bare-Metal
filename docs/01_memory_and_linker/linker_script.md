# Memory Layout & Linker Script

## Overview

When you compile C code for a desktop, the OS loader handles placing your program in memory. On a bare-metal microcontroller, **there is no OS** — the linker script is your memory manager. It tells the GNU linker (`ld`) exactly where every byte of your firmware lives in the chip's address space.

Without a linker script, the linker has no idea that:

- Flash starts at `0x0800_0000` (not `0x0000_0000`)
- RAM starts at `0x2000_0000`
- The very first word in Flash must be the initial stack pointer
- Initialized global variables need to be copied from Flash to RAM at boot

The linker script answers all of these questions.

---

## STM32F401 Memory Map

The Cortex-M4 uses a fixed 4 GB address space. The STM32F401CCU6 populates it like this:

```
  0xFFFF_FFFF ┌──────────────────────────┐
              │   Cortex-M4 Internal     │
              │   (NVIC, SysTick, SCB)   │
  0xE000_0000 ├──────────────────────────┤
              │                          │
              │   (Reserved / Unused)    │
              │                          │
  0x4002_6400 ├──────────────────────────┤
              │   AHB2 Peripherals       │
              │   (USB OTG FS)           │
  0x4002_0000 ├──────────────────────────┤
              │   AHB1 Peripherals       │
              │   (GPIO, RCC, DMA, ...)  │
  0x4001_0000 ├──────────────────────────┤
              │   APB2 Peripherals       │
              │   (USART1, SPI1, TIM1..) │
  0x4000_8000 ├──────────────────────────┤
              │   APB1 Peripherals       │
              │   (USART2, I2C, TIM2..)  │
  0x4000_0000 ├──────────────────────────┤
              │                          │
              │   (Reserved)             │
              │                          │
  0x2001_8000 ├──────────────────────────┤
              │▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓│
              │▓▓▓▓  SRAM (96 KB)  ▓▓▓▓▓▓│  ← .data, .bss, stack, heap
              │▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓│
  0x2000_0000 ├──────────────────────────┤
              │                          │
              │   (Reserved)             │
              │                          │
  0x0806_0000 ├──────────────────────────┤
              │██████████████████████████│
              │████  Flash (384 KB) █████│  ← .vectors, .text, .rodata, .data (LMA)
              │██████████████████████████│
  0x0800_0000 ├──────────────────────────┤
              │   Aliased to Flash       │
              │   (boot area)            │
  0x0000_0000 └──────────────────────────┘
```

!!! note
    The linker script declares 384K of Flash and 96K of RAM, matching the maximum for the STM32F401xB/C family. The STM32F401CCU6 specifically has 256 KB Flash and 64 KB SRAM — the extra space is simply unmapped. Using the larger values keeps the script compatible if you ever swap to a larger variant (e.g., STM32F401xD/E).

---

## The Complete Linker Script

Here is the full `linker.ld` with section-by-section analysis.

### Entry Point

```ld
ENTRY(reset_handler)
```

This tells the linker which symbol is the program's entry point. Debuggers (GDB, OpenOCD) use this to know where to set the initial program counter. On a real Cortex-M4, the hardware ignores this — it reads the reset vector from the vector table at address `0x0800_0004`. But specifying it keeps the linker from discarding `reset_handler` as unused, and makes debugging tools happy.

---

### Memory Regions

```ld
MEMORY {
    ROM (rx)  : ORIGIN = 0x08000000, LENGTH = 384K
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 96K
}
```

| Region | Attributes | Start Address | Size | Contents |
|--------|-----------|---------------|------|----------|
| `ROM` | `r` read, `x` execute | `0x0800_0000` | 384 KB | Vector table, code, constants, initial values of `.data` |
| `RAM` | `r` read, `w` write, `x` execute | `0x2000_0000` | 96 KB | Global variables, BSS, stack, heap |

The attributes (`rx`, `rwx`) are hints to the linker. If you accidentally try to place a writable section into `ROM`, the linker will emit a warning.

---

### Stack Pointer Initialization

```ld
_estack = ORIGIN(RAM) + LENGTH(RAM);
```

This creates a symbol `_estack` that points to the **top of RAM** — the highest address plus one. On the Cortex-M4, the stack grows **downward** (from high addresses to low), so the initial stack pointer should be the end of RAM.

```
  RAM (96 KB)
  0x2001_8000  ← _estack (initial SP goes here)
      │  ▲ stack grows downward
      │  │
      │  │
      │
  0x2000_0000  ← _sdata, _sbss (heap/globals grow upward)
```

This symbol is placed as the **very first word** in the vector table (offset `0x00`), which is where the Cortex-M4 hardware reads the initial stack pointer value from at reset.

---

### Sections

#### `.vectors` — Vector Table

```ld
.vectors : {
    KEEP(*(.vectors))
    . = ALIGN(4);
} > ROM
```

- **`KEEP`**: Prevents the linker from discarding the vector table during garbage collection (`--gc-sections`). The vector table is referenced by hardware, not by code, so without `KEEP` the linker would think it's unused and throw it away.
- **`> ROM`**: Places this section at the start of Flash (`0x0800_0000`). The Cortex-M4 **requires** the vector table to begin at the base of the boot memory region.
- **`ALIGN(4)`**: Ensures 4-byte alignment. ARM instructions and data accesses must be word-aligned.

---

#### `.text` — Executable Code

```ld
.text : {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    . = ALIGN(4);
} > ROM
```

All compiled function bodies go here. The `*(.text*)` wildcard catches compiler-generated subsections like `.text.startup` or `.text.unlikely` (used by GCC's `-ffunction-sections`).

---

#### `.rodata` — Read-Only Data

```ld
.rodata : {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
} > ROM
```

String literals (`"Hello"`) and `const` global variables are placed here. They live in Flash because they never need to be written.

---

#### `.data` — Initialized Global Variables

```ld
_sidata = LOADADDR(.data);

.data : {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
} > RAM AT > ROM
```

This is the most critical section to understand. It has **two addresses**:

| Address Type | Meaning | Value |
|---|---|---|
| **VMA** (Virtual Memory Address) | Where the CPU reads/writes the variable at runtime | In RAM (`0x2000_xxxx`) |
| **LMA** (Load Memory Address) | Where the initial values are stored in the binary | In ROM (`0x0800_xxxx`) |

**Why two addresses?** A global variable like `int counter = 42;` needs to:

1. **Survive power loss** → its initial value (`42`) is stored in Flash (non-volatile)
2. **Be writable at runtime** → the CPU accesses it in RAM (volatile)

So at boot, the startup code must **copy** the initial values from Flash (LMA) to RAM (VMA). Three symbols make this possible:

| Symbol | Meaning |
|--------|---------|
| `_sidata` | Source address — start of `.data` initial values in Flash (LMA) |
| `_sdata` | Destination start — beginning of `.data` in RAM (VMA) |
| `_edata` | Destination end — end of `.data` in RAM (VMA) |

```
  Flash (ROM)                          RAM
  ┌──────────────┐                   ┌──────────────┐
  │  .vectors    │                   │              │
  │  .text       │                   │              │
  │  .rodata     │                   │              │
  ├──────────────┤                   ├──────────────┤
  │  .data (LMA) │ ──── copy ────▶  │  .data (VMA) │
  │  _sidata     │    at boot        │  _sdata      │
  │              │                   │  _edata      │
  └──────────────┘                   ├──────────────┤
                                     │  .bss        │
                                     │  _sbss       │
                                     │  _ebss       │
                                     ├──────────────┤
                                     │     ▲        │
                                     │   stack      │
                                     │              │
                                     │  _estack     │
                                     └──────────────┘
```

---

#### `.bss` — Zero-Initialized Variables

```ld
.bss : {
    . = ALIGN(4);
    _sbss = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
} > RAM
```

Uninitialized or zero-initialized global variables (`static int x;`, `int arr[100] = {0};`) go here. The `.bss` section takes up **zero space in the binary** — only RAM is reserved. The startup code zeros this region using `_sbss` and `_ebss`.

`*(COMMON)` catches old-style C "tentative definitions" (e.g., `int x;` in multiple files without `extern`).

---

#### `/DISCARD/` — Throw Away Unwanted Sections

```ld
/DISCARD/ : {
    *(.comment)
}
```

The `.comment` section contains compiler version strings. It's useless on an embedded target and wastes Flash, so we discard it.

---

## Key Takeaways

!!! tip "Key Takeaways"
    1. **The linker script is the memory map** — it tells the linker where Flash and RAM are, and which sections go where.
    2. **`_estack`** is placed at the top of RAM and becomes the initial stack pointer (first word of the vector table).
    3. **`.data` has two addresses** — LMA in Flash (where initial values are stored) and VMA in RAM (where the CPU accesses them). The startup code copies from one to the other.
    4. **`.bss` costs zero Flash** — it only reserves RAM space that the startup code fills with zeros.
    5. **`KEEP`** is essential for the vector table — without it, link-time garbage collection would discard hardware-referenced data.
    6. **Symbols like `_sdata`, `_edata`, `_sidata`, `_sbss`, `_ebss`** are the contract between the linker script and the startup assembly — they must match exactly.
