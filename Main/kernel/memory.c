#include "common.h"
#include "console.h"
#include "memory.h"

/*
 * Simple bump allocator.
 *
 * This does allocation, but not free().
 * It is simple, predictable, and appropriate for this project scope.
 */

#define HEAP_START 0x00100000  /* 1MB */
#define HEAP_END   0x00400000  /* 4MB */

static u32 heap_current = HEAP_START;

static u32 align_4(u32 value) {
    return (value + 3) & ~3;
}

void memory_init(void) {
    heap_current = HEAP_START;

    console_write("Memory allocator initialized.\n");
    console_write("Heap start: ");
    console_write_hex(HEAP_START);
    console_write("\n");
}

void* kmalloc(u32 size) {
    if (size == 0) {
        return NULL;
    }

    size = align_4(size);

    if (heap_current + size >= HEAP_END) {
        return NULL;
    }

    u32 allocated_address = heap_current;
    heap_current += size;

    return (void*)allocated_address;
}

u32 memory_get_heap_start(void) {
    return HEAP_START;
}

u32 memory_get_heap_current(void) {
    return heap_current;
}

u32 memory_get_heap_used(void) {
    return heap_current - HEAP_START;
}