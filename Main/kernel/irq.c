#include "common.h"
#include "console.h"
#include "irq.h"
#include "panic.h"

/* handlers for IRQ0..IRQ15 */
static irq_handler_t irq_routines[16] = { 0 };

void irq_install_handler(int irq, irq_handler_t handler) {
    if (irq >= 0 && irq < 16) {
        irq_routines[irq] = handler;
    }
}

static void pic_remap(void) {
    outb(0x20, 0x11);
    io_wait();
    outb(0xA0, 0x11);
    io_wait();

    outb(0x21, 0x20);
    io_wait();
    outb(0xA1, 0x28);
    io_wait();

    outb(0x21, 0x04);
    io_wait();
    outb(0xA1, 0x02);
    io_wait();

    outb(0x21, 0x01);
    io_wait();
    outb(0xA1, 0x01);
    io_wait();

    outb(0x21, 0x00);
    outb(0xA1, 0x00);
}

void irq_init(void) {
    pic_remap();
}

static const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved"
};

void isr_handler(u32 int_no, u32 err_code) {
    (void)err_code;

    console_write("\nEXCEPTION: ");

    if (int_no < 32) {
        console_write(exception_messages[int_no]);
    } else {
        console_write("Unknown");
    }

    console_write(" (");
    console_write_dec(int_no);
    console_write(")\n");

    panic("CPU exception occurred");
}

void irq_handler(u32 int_no, u32 err_code) {
    (void)err_code;

    if (int_no >= 40) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);

    int irq = (int)int_no - 32;

    if (irq >= 0 && irq < 16 && irq_routines[irq]) {
        irq_routines[irq]();
    }
}