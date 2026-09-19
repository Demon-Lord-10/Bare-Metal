ENTRY(reset_handler)

MEMORY {
    ROM (rx)  : ORIGIN = 0x08000000, LENGTH = 384K
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 96K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS {
    .vectors : {
        KEEP(*(.vectors))
        . = ALIGN(4);
    } > ROM

    .text : {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        . = ALIGN(4);
    } > ROM

    .rodata : {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } > ROM

    _sidata = LOADADDR(.data);

    .data : {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT > ROM

    .bss : {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM

    /DISCARD/ : {
        *(.comment)
    }
}
