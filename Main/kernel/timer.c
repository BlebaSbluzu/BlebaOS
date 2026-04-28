#include "common.h"
#include "console.h"
#include "irq.h"
#include "timer.h"
#include "scheduler.h"
static volatile u32 timer_ticks = 0;

static void timer_callback(void) {
    timer_ticks++;

    scheduler_tick();
    // if ((timer_ticks % 100) == 0) {
    //     console_write("[TICK ");
    //     console_write_dec(timer_ticks);
    //     console_write("]\n");
    // }
}

void timer_init(u32 frequency) {
    u32 divisor = 1193180 / frequency;

    irq_install_handler(0, timer_callback);

    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

u32 timer_get_ticks(void) {
    return timer_ticks;
}