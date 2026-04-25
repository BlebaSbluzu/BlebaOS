#include "console.h"
#include "panic.h"

void panic(const char* message) {
    console_write("\n\n*** KERNEL PANIC ***\n");
    console_write(message);
    console_write("\nSystem halted.\n");

    for (;;) {
        __asm__ volatile ("cli; hlt");
    }
}