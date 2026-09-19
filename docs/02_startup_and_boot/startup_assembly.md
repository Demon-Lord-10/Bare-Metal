# Startup Code & Boot Sequence

## Overview

On a desktop PC, the operating system sets up your program's stack, loads globals into memory, and calls `main()`. On a bare-metal Cortex-M4, **there is no OS** — a small piece of hand-written assembly must do all of this before your C code can run.

This page walks through `startup.s` line by line and explains exactly what happens between the moment power is applied and the moment `main()` executes.

---

## The Boot Sequence

When the STM32F401 powers on (or resets), the Cortex-M4 core performs a fixed hardware sequence:

```
     Power On / Reset
           │
           ▼
  ┌─────────────────────────────┐
  │ 1. Read word at 0x0800_0000 │──▶ Load into SP (Stack Pointer)
  └─────────────────────────────┘
           │
           ▼
  ┌─────────────────────────────┐
  │ 2. Read word at 0x0800_0004 │──▶ Load into PC (Program Counter)
  └─────────────────────────────┘
           │
           ▼
  ┌─────────────────────────────┐
  │ 3. Begin execution at PC    │──▶ Jumps to reset_handler
  └─────────────────────────────┘
           │
           ▼
  ┌─────────────────────────────┐
  │ 4. Copy .data from Flash    │
  │    to RAM (LMA → VMA)       │
  └─────────────────────────────┘
           │
           ▼
  ┌─────────────────────────────┐
  │ 5. Zero .bss in RAM         │
  └─────────────────────────────┘
           │
           ▼
  ┌─────────────────────────────┐
  │ 6. Branch to main()         │
  └─────────────────────────────┘
```

Steps 1–3 are done by **hardware** — no software involved. Steps 4–6 are done by our **startup assembly code**.

!!! note
    This is fundamentally different from ARM-A series (e.g., Raspberry Pi) where the core starts in ARM mode and jumps to a fixed address. Cortex-M uses a **vector table fetch** — the hardware reads a table of addresses, not instructions. The first entry isn't even code — it's the initial stack pointer value.

---

## The Complete Startup Code

Here is the full `startup.s` with every line explained.

### Processor & Instruction Set Directives

```asm
.cpu cortex-m4
.thumb
```

| Directive | Meaning |
|-----------|---------|
| `.cpu cortex-m4` | Tell the assembler to accept only instructions valid on the Cortex-M4 (ARMv7E-M architecture).<br>This enables DSP instructions and optional FPU instructions. |
| `.thumb` | Use the **Thumb-2** instruction set. Cortex-M4 *only* supports Thumb/Thumb-2 — it cannot execute classic 32-bit ARM instructions.<br>All vector table entries must have bit 0 set (indicating Thumb mode), which the assembler handles automatically. |

---

### Global and External Symbols

```asm
.global reset_handler
.global systick_handler
.extern main
```

| Directive | Meaning |
|-----------|---------|
| `.global reset_handler` | Export `reset_handler` so the linker can see it. The linker script references it as `ENTRY(reset_handler)`. |
| `.global systick_handler` | Export `systick_handler` so C code can override the weak default. |
| `.extern main` | Declare that `main` is defined elsewhere (in a `.c` file). The `bl main` instruction at the end needs this symbol to be resolved by the linker. |

---

### Vector Table

```asm
.section .vectors, "a", %progbits
vector_table:
    .word _estack
    .word reset_handler
    .org 0x3C
    .word systick_handler
```

This is the most hardware-critical part of the entire firmware. The Cortex-M4 vector table format is defined by ARM:

| Offset | Vector # | Contents | Our Value |
|--------|----------|----------|-----------|
| `0x00` | — | Initial Stack Pointer | `_estack` (top of RAM) |
| `0x04` | 1 | Reset handler | `reset_handler` |
| `0x08` | 2 | NMI handler | *(not set — defaults to 0)* |
| `0x0C` | 3 | HardFault handler | *(not set)* |
| `0x10` | 4 | MemManage handler | *(not set)* |
| `0x14` | 5 | BusFault handler | *(not set)* |
| `0x18` | 6 | UsageFault handler | *(not set)* |
| `0x1C`–`0x28` | 7–10 | Reserved | *(not set)* |
| `0x2C` | 11 | SVCall handler | *(not set)* |
| `0x30` | 12 | Debug Monitor | *(not set)* |
| `0x34` | 13 | Reserved | *(not set)* |
| `0x38` | 14 | PendSV handler | *(not set)* |
| **`0x3C`** | **15** | **SysTick handler** | **`systick_handler`** |
| `0x40`+ | 16+ | IRQ0, IRQ1, ... | *(not set)* |

**Key details:**

- **`_estack` at offset 0x00**: This is NOT an instruction — it's a 32-bit value. The hardware loads this into the Main Stack Pointer (MSP) register at reset. Since ARM stacks grow downward, we point it to the top of RAM.

- **`.org 0x3C`**: Jump the location counter to offset `0x3C` within the section, skipping over the unused entries (NMI, HardFault, etc.). The skipped entries are filled with zeros. If any of those exceptions fire, the core will try to jump to address `0x0000_0000` — which on STM32 is aliased to Flash, effectively causing a reset.

- **`systick_handler` at offset 0x3C**: The SysTick timer is a core peripheral (not an STM32-specific IRQ), so its vector is at a fixed position in the system exception table.

!!! warning
    The unused vector entries (NMI, HardFault, etc.) are zero. If a HardFault occurs, the core will jump to address `0x0` — which may silently restart the firmware or lock up. In production, you'd want fault handlers that at least enter an infinite loop or log a diagnostic.

---

### The Reset Handler

This is the code that runs **first** after every reset. Its job is to set up the C runtime environment.

#### Step 1: Copy `.data` from Flash to RAM

```asm
.section .text
.align 1

.type reset_handler, %function
reset_handler:
    /* Copy .data from ROM -> RAM */
    ldr r0, =_sdata       @ r0 = destination start (RAM)
    ldr r1, =_edata       @ r1 = destination end (RAM)
    ldr r2, =_sidata      @ r2 = source start (Flash)

data_loop:
    cmp r0, r1            @ Have we reached the end?
    bge data_done          @ If yes, stop copying
    ldr r3, [r2], #4      @ Load word from Flash, post-increment r2
    str r3, [r0], #4      @ Store word to RAM, post-increment r0
    b data_loop            @ Repeat
```

This loop copies the initial values of global variables from Flash (where they're stored in the binary) to RAM (where the CPU will read/write them at runtime). The symbols `_sdata`, `_edata`, and `_sidata` are defined in the linker script.

**Register usage:**

| Register | Role | Linker Symbol |
|----------|------|---------------|
| `r0` | Destination pointer (RAM), increments each iteration | `_sdata` → `_edata` |
| `r1` | Destination end address (RAM) | `_edata` |
| `r2` | Source pointer (Flash), increments each iteration | `_sidata` |
| `r3` | Temporary — holds the word being copied | — |

**The `[r2], #4` syntax**: This is ARM **post-indexed** addressing. It means: load from the address in `r2`, *then* add 4 to `r2`. This is equivalent to `*src++` in C.

---

#### Step 2: Zero the `.bss` Section

```asm
data_done:
    /* Zero BSS */
    ldr r0, =_sbss        @ r0 = BSS start (RAM)
    ldr r1, =_ebss        @ r1 = BSS end (RAM)
    mov r2, #0             @ r2 = 0 (the value to fill with)

bss_loop:
    cmp r0, r1            @ Have we reached the end?
    bge bss_done           @ If yes, stop zeroing
    str r2, [r0], #4      @ Store 0 to RAM, post-increment r0
    b bss_loop             @ Repeat
```

The C standard guarantees that uninitialized global and static variables are zero. Since `.bss` takes up zero space in the Flash binary (to save space), we must explicitly fill that RAM region with zeros.

---

#### Step 3: Call `main()` and Hang

```asm
bss_done:
    bl main               @ Branch-with-Link to main()
    b .                    @ Infinite loop (should never reach here)
```

- **`bl main`**: Branch with Link — this calls `main()` and stores the return address in the Link Register (`LR`). If `main()` ever returns (which it shouldn't in embedded), execution falls through to the next instruction.

- **`b .`**: Branch to self — an infinite loop. This is a safety net. If `main()` returns, the CPU loops here forever instead of executing random memory. In production, you might trigger a system reset here instead.

---

### Default SysTick Handler

```asm
.weak systick_handler
.type systick_handler, %function
systick_handler:
    b .                    @ Infinite loop (default: do nothing)
```

| Directive | Meaning |
|-----------|---------|
| `.weak` | Marks this as a **weak symbol**. If any C file defines `void systick_handler(void)`, the linker will use that definition instead of this one. This pattern lets you provide a safe default that can be overridden without linker errors. |
| `b .` | The default handler just loops forever. If SysTick fires and you haven't written a real handler, the CPU stalls here — annoying but safe (no wild jumps, no corrupted memory). |

---

## How Linker & Startup Work Together

The linker script and startup assembly are tightly coupled through **symbols**. Here's the contract:

```
  Linker Script (linker.ld)              Startup Assembly (startup.s)
  ─────────────────────────              ──────────────────────────────
  _estack = top of RAM         ───▶     .word _estack        (vector table, offset 0x00)
  ENTRY(reset_handler)         ◀───     .global reset_handler

  _sidata = LOADADDR(.data)    ───▶     ldr r2, =_sidata     (Flash source address)
  _sdata  = start of .data     ───▶     ldr r0, =_sdata      (RAM destination start)
  _edata  = end of .data       ───▶     ldr r1, =_edata      (RAM destination end)

  _sbss   = start of .bss      ───▶     ldr r0, =_sbss       (BSS start)
  _ebss   = end of .bss        ───▶     ldr r1, =_ebss       (BSS end)
```

If you rename a symbol in one file but not the other, the linker will fail with an **"undefined reference"** error. These two files are a matched pair.

---

## Key Takeaways

!!! tip "Key Takeaways"
    1. **The Cortex-M4 boot is table-driven** — the hardware reads an initial SP and a reset vector from Flash. It does not execute the first instruction at address zero.
    2. **The startup code is the C runtime** — it's responsible for the guarantees that C programmers take for granted (initialized globals, zeroed BSS).
    3. **Three loops before `main()`**: read the vector table (hardware), copy `.data` (software), zero `.bss` (software).
    4. **Weak symbols** let you provide safe defaults that C code can override — essential for interrupt handlers.
    5. **`.thumb` is mandatory** — Cortex-M4 does not support ARM mode. Forgetting this directive produces illegal instructions.
    6. **`b .` is your safety net** — after `main()` and in default handlers, an infinite loop prevents the core from running off into random memory.
