#ifndef TIMER_H
#define TIMER_H

#include "common.h"

void timer_init(u32 frequency);
u32 timer_get_ticks(void);

#endif