; kernel.asm - 32-bit "kernel" with nicer screen
; - Runs in 32-bit protected mode
; - Clears screen to blue background
; - Prints a simple centered banner

BITS 32
ORG 0x00010000          ; must match KERNEL_ENTRY in boot.asm

%define VGA_ADDR     0xB8000
%define SCREEN_COLS  80
%define SCREEN_ROWS  25
%define ATTR         0x1F      ; fg = bright white (0xF), bg = blue (0x1)

kernel_entry:
    ; ----------------------------
    ; 1) Clear screen
    ; ----------------------------
    mov edi, VGA_ADDR           ; destination in VGA text memory
    mov ecx, SCREEN_COLS * SCREEN_ROWS
    mov ax, (ATTR << 8) | ' '   ; space char + colour attribute

    cld
.clear_loop:
    stosw                       ; write AX, advance EDI by 2
    loop .clear_loop

    ; ----------------------------
    ; 2) Print title "BlebaOS"
    ;    at row 5, column 30
    ; ----------------------------
    mov esi, title_msg
    mov dh, 5                   ; row
    mov dl, 30                  ; column
    call print_at

    ; ----------------------------
    ; 3) Print status line
    ;    "Bootloader → Protected Mode OK"
    ;    at row 7, column 15
    ; ----------------------------
    mov esi, status_msg
    mov dh, 7
    mov dl, 15
    call print_at

    ; ----------------------------
    ; 4) Hang
    ; ----------------------------
.hang:
    hlt
    jmp .hang

; ------------------------------------
; print_at:
;   ESI = pointer to zero-terminated string
;   DH  = row (0–24)
;   DL  = column (0–79)
; Uses VGA_ADDR and ATTR
; ------------------------------------
print_at:
    push eax
    push ebx
    push ecx
    push edx
    push edi

    ; Compute offset = (row * 80 + col) * 2
    movzx eax, dh               ; row
    movzx ebx, dl               ; col

    imul eax, SCREEN_COLS       ; row * 80
    add  eax, ebx               ; row*80 + col
    shl  eax, 1                 ; *2 bytes per cell

    mov edi, VGA_ADDR
    add edi, eax                ; EDI = VGA base + offset

.print_loop:
    lodsb                       ; AL = [ESI], ESI++
    test al, al
    jz   .done
    mov ah, ATTR
    mov [edi], ax               ; write char + attr
    add edi, 2
    jmp .print_loop

.done:
    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

; ------------------------------------
; Strings
; ------------------------------------
title_msg  db "BlebaOS", 0
status_msg db "Bootloader -> Protected mode -> Kernel entry OK", 0