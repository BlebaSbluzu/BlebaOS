BITS 32

global idt_load
extern isr_handler
extern irq_handler

idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ret

%macro ISR 1
global isr%1
isr%1:
    cli
    push dword 0
    push dword %1
    call isr_handler
    add esp, 8
    sti
    iret
%endmacro

ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR 8
ISR 9
ISR 10
ISR 11
ISR 12
ISR 13
ISR 14
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19
ISR 20
ISR 21
ISR 22
ISR 23
ISR 24
ISR 25
ISR 26
ISR 27
ISR 28
ISR 29
ISR 30
ISR 31

global irq0
irq0:
    cli
    push dword 0
    push dword 32
    call irq_handler
    add esp, 8
    sti
    iret

global irq1
irq1:
    cli
    push dword 0
    push dword 33
    call irq_handler
    add esp, 8
    sti
    iret