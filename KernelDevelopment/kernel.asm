[BITS 32]
[extern main] ; This tells NASM that 'main' is defined in another file (like C)

start:
    [extern main] ;
    call main  ; Call our C entry point
    jmp $      ; If main returns, halt the CPU in an infinite loop