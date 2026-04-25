BITS 32

global _start
extern kernel_main

section .text

_start:
    cli                 ; keep interrupts off during very early kernel init
    cld                 ; clear direction flag for C code safety

    ; Set up a fresh stack for the kernel
    mov esp, 0x90000

    ; Call the C kernel entry point
    call kernel_main

.hang:
    cli
    hlt
    jmp .hang