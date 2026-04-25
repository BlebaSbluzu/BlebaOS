BITS 16
ORG 0x7C00                      ; BIOS loads us here

KERNEL_LOAD_SEG   EQU 0x1000    ; real-mode segment where we load kernel
KERNEL_ENTRY      EQU 0x00010000 ; full 32 bit address for kernel entry (protected)
KERNEL_SECTORS    EQU 20       ; how many sectors of kernel to read 



; =========================
; Boot entry point
; =========================
start:
    cli                         ; disables interrupts "Clear interrupts flag"

    xor ax, ax                  ;Set the segments to 0
    mov ds, ax                  ; data segment 
    mov es, ax
    mov ss, ax                  ; stack segment 
    mov sp, 0x7C00              ; stack grows down from here - stack pointer

    sti                         ; enable interrupts again

    mov [boot_drive], dl        ; SET DL TO BIOS BOOT_DRIVE VARIABLE

    ; Print basic status
    mov si, msg_boot  ;si is "source index register" we load offset of msg_boot into it here.
    call print_string

    ;Enable A20 so we can use memory above 1MB
    call enable_a20

    ;Load kernel sectors into memory
    mov si, msg_load
    call print_string
    call load_kernel

    ;Enter protected mode (GDT + CR0.PE + far jump)
    mov si, msg_pmode
    call print_string
    call enter_protected_mode  ; if we ret here which we shouldnt we go to hang below which halts the cpu until the next interrupt

hang:
    hlt
    jmp hang

; =========================
; Enable A20 via port 0x92
; (fast A20 gate method)
; =========================
enable_a20:
    in   al, 0x92               ; read byte from hardware i/o port 0x92 into AL
    test al, 00000010b          ; tests if al is already enabled? if al is 1, zero flag is 0
    jnz  .done                  ;Jump if not zero 
    or   al, 00000010b          ; set A20 enable bit, now is 1
    and  al, 11111110b          ; clear reset bit
    out  0x92, al
.done:
    ret

; =========================
; Load kernel from disk using BIOS INT 13h
; - Assumes floppy / simple CHS: cylinder 0, head 0, sector 2+
; - Loads KERNEL_SECTORS sectors starting at sector 2
; - Loads to segment KERNEL_LOAD_SEG, offset 0
; =========================
load_kernel:
    mov ax, KERNEL_LOAD_SEG     ; ES:BX = destination, ES register is for extra data segments,BX offset register  
    ;ES:BX is the pointer BIOS INT 13h uses as the memory destination when loading sectors
    
    mov es, ax  ;put segment into ES
    xor bx, bx  ; bx the offset is 0

    mov ah, 0x02                ; INT 13h function 02h - read sectors
    mov al, KERNEL_SECTORS      ; number of sectors to read
    mov ch, 0                   ; cylinder 0
    mov cl, 2                   ; starting at sector 2 (sector 1 = boot)
    mov dh, 0                   ; disk head 0
    mov dl, [boot_drive]        ; store BIOS boot drive number (DL) into the variable boot_drive

    int 0x13                    ; BIOS disk read
    jc disk_error               ; jump if carry flag set -> error

    ret

disk_error:
    mov si, msg_disk
    call print_string
    jmp hang

; =========================
; Print string at DS:SI using BIOS teletype (INT 10h/AH=0Eh)
; String must be zero-terminated.
; =========================
print_string:
    pusha         ; pushes all general purpose registers to stack
.print_next:
    lodsb                       ; AL = [DS:SI], SI++ so we loop through the string "LOaD string Byte", lodsb loads from DS:SI
    or  al, al              ; checks if al = 0
    jz  .done               ; if al is 0 jump to done "jump if zero"
    mov ah, 0x0E                ; function 0x0E prints the value of AL to screen
    mov bh, 0x00
    mov bl, 0x07                ; attribute (grey on black)
    int 0x10
    jmp .print_next
.done:
    popa        ;restor the registers how they were before print_string
    ret

; =========================
; Enter 32-bit protected mode
; Steps:
; 1. Load GDT
; 2. Set PE bit in CR0
; 3. Far jump to 32-bit entry in code segment selector 0x08
; =========================
enter_protected_mode:
    cli                 ;disable interrupts again while changing mode
    lgdt [gdt_descriptor]       ; load GDT (code + data descriptors) gdt_descriptor structure defined later

    mov eax, cr0
    or  eax, 1                  ; set PE bit (Protection Enable)
    mov cr0, eax

    ; Far jump: loads CS with 0x08 (code segment) and EIP(Extended instruction Pointer) with label

    ;EIP is bigger IP (instruction pointer)

    jmp 0x08:protected_entry

; =========================
; 32-bit entry stub
; Now we are in protected mode with paging disabled.
; CS = 0x08 (code segment descriptor), base 0, 32-bit.
; We set up 32-bit data segments & stack, then jump to kernel.
; =========================
BITS 32 ;TELL NASM WERE IN 32 BIT NOW 
protected_entry:
    ; Load data segment registers with data selector (0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    ; Set up a simple 32-bit stack
    mov esp, 0x0090000          ; arbitrary stack location below 1MB

    ; Jump to kernel entry point (which we loaded at 0x00010000)
    jmp KERNEL_ENTRY




; =========================
; Global Descriptor Table (GDT)
; =========================
; Descriptor format: limit, base, flags
; We define:
;  - null descriptor
;  - 32-bit code segment (selector 0x08)
;  - 32-bit data segment (selector 0x10)
; GDT layout (each entry = 8 bytes):
;   0x00 → Null descriptor (required)
;   0x08 → Code segment (base=0, limit=4GB)
;   0x10 → Data segment (base=0, limit=4GB)
;
; Segment selector = index * 8
;   0x08 = 2nd entry = code segment
;   0x10 = 3rd entry = data segment
BITS 16                        ; (BITS here only matters for instructions; data is the same)

gdt_start:
gdt_null:
    dq 0x0000000000000000      ; null descriptor

gdt_code:
    dw 0xFFFF                  ; limit low
    dw 0x0000                  ; base low
    db 0x00                    ; base middle
    db 10011010b               ; access: present, ring 0, code, readable
    db 11001111b               ; flags: 4KB granularity, 32-bit, high limit bits
    db 0x00                    ; base high

gdt_data:
    dw 0xFFFF                  ; limit low
    dw 0x0000                  ; base low
    db 0x00                    ; base middle
    db 10010010b               ; access: present, ring 0, data, writable
    db 11001111b               ; flags: 4KB granularity, 32-bit
    db 0x00                    ; base high

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; size of GDT
    dd gdt_start               ; linear address of GDT

; =========================
; Data
; =========================
boot_drive db 0

msg_boot db "Bootloader: starting...", 0
msg_load db 13,10,"Loading kernel...", 0
msg_pmode db 13,10,"Switching to protected mode...", 0
msg_disk db 13,10,"Disk read error!", 0

; =========================
; Boot sector signature
; =========================
times 510 - ($ - $$) db 0      ; pad up to 510 bytes
dw 0xAA55                      ; boot signature
