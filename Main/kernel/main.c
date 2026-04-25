#include "console.h"
#include "idt.h"
#include "irq.h"
#include "timer.h"
#include "keyboard.h"
#include "shell.h"

void kernel_main(void) {
    console_init();
    console_write("BlebaOS kernel started.\n");
    console_write("Stage 4 bring-up...\n");

    idt_init();
    irq_init();
    timer_init(100);
    keyboard_init();

    console_write("IDT loaded.\n");
    console_write("PIC remapped.\n");
    console_write("Timer running.\n");
    console_write("Keyboard ready.\n");
    console_write("Interrupts enabled.\n");

    __asm__ volatile ("sti");

    shell_init();

    for (;;) {
        __asm__ volatile ("hlt");
    }
}