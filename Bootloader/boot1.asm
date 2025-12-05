; Instruct NASM to generate code that is to be run on CPU that is running in 16 bit mode
bits 16

; Infinite loop
loop:
    jmp loop

; Fill remaining space of the 512 bytes minus our instrunctions, with 00 bytes
; $ - address of the current instruction
; $$ - address of the start of the image .text section we're executing this code in
times 510 - ($-$$) db 0
; Bootloader magic number
dw 0xaa55