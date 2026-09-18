ENTRY(reset_handler)

MEMORY{
    ROM(rx)  : ORIGIN = 0x08000000 , LENGTH = 384K
    RAM(rwx) : ORIGIN = 0x20000000 , LENGTH = 96K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS{
    .vectors :{
        KEEP(*(.vectors))
    } > ROM

    .text :{
        .=ALIGN(4);
        *(.text)
        *(.text*)
    } > ROM

    .rodata :{
        *(.rodata)
        *(.rodta*)
    } > ROM

    _sidata = LOADADDR(.data);

    .data :{
        _sdata = .;
        *(.data)
        *(.data*)
        _edata = .;
    } > RAM AT > ROM

    .bss :{
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > RAM

    /DISCARD/ : {
        *(.comment)
    }

}
