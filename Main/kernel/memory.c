#include "common.h"
#include "console.h"
#include "memory.h"


#define HEAP_START 0x00100000  
#define HEAP_END   0x00400000  

static u32 heap_current = HEAP_START;

static u32 align_4(u32 value) {
    u32 remainder = value % 4;

    // Helps with 32 bit. If value is already divisible by 4, keep it.
    // Otherwise, add enough bytes to reach the next multiple of 4.
    //basically makes sure the value fits into a the 4 byte wide 
    // 32 bit value in the 32 bit system (its neater)

  if (remainder == 0) {
        return value;
    }

    return value + (4 - remainder);
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