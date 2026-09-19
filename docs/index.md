# Linker Script Architecture

The linker script maps compiler output sections into the STM32F401CCU6 physical memory space.

## Memory Mapping

```ld
MEMORY {
    ROM(rx)  : ORIGIN = 0x08000000, LENGTH = 384K
    RAM(rwx) : ORIGIN = 0x20000000, LENGTH = 96K
}
```

- **ROM (Flash)**: Non-volatile memory starting at `0x08000000`. Stores vector tables, `.text`, `.rodata`, and the LMA of `.data`.
- **RAM (SRAM)**: Volatile memory starting at `0x20000000`. Stores stack, heap, `.bss`, and the VMA of `.data`.
