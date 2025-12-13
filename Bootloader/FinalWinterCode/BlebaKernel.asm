
BITS 32
ORG 0x00010000          ; must match KERNEL_ENTRY in boot.asm

kernel_entry:
    mov esi, msg
    mov edi, 0xB8000    ; VGA text buffer
    mov ah, 0x07        ; text attribute (grey on black)

.print_loop:
    lodsb               ; AL = [ESI], ESI++
    test al, al
    jz .done
    mov [edi], al       ; character
    mov [edi+1], ah     ; attribute
    add edi, 2
    jmp .print_loop

.done:
    ; Hang the CPU
.hang:
    hlt
    jmp .hang

msg db "Welcome to BlebaOS - coolest OS of all time", 0
