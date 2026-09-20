.cpu cortex-m4
.syntax unified
.thumb

.global reset_handler
.global systick_handler
.extern main

.section .vectors, "a", %progbits
vector_table:
    .word _estack
    .word reset_handler
    .org 0x3C
    .word systick_handler

.section .text
.align 1

.type reset_handler, %function
reset_handler:
    /*Copy .data from ROM -> RAM*/
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata

data_loop:
    cmp r0,r1
    bge data_done
    ldr r3, [r2], #4
    str r3, [r0], #4
    b data_loop

data_done:
    /*Zero BSS*/
    ldr r0, =_sbss
    ldr r1, =_ebss
    mov r2, #0

bss_loop:
    cmp r0,r1
    bge bss_done
    str r2, [r0], #4
    b bss_loop

bss_done:
    bl main
    b .

.weak systick_handler
.type systick_handler, %function
systick_handler:
    b .
