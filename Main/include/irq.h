#ifndef IRQ_H
#define IRQ_H

#include "common.h"

typedef void (*irq_handler_t)(void);

void irq_init(void);
void irq_install_handler(int irq, irq_handler_t handler);

void isr_handler(u32 int_no, u32 err_code);
void irq_handler(u32 int_no, u32 err_code);

#endif