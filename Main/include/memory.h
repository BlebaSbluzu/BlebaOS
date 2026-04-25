#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"

void memory_init(void);
void* kmalloc(u32 size);
u32 memory_get_heap_start(void);
u32 memory_get_heap_current(void);
u32 memory_get_heap_used(void);

#endif