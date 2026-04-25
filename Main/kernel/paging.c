#include "common.h"
#include "console.h"
#include "paging.h"

/*
 * Legacy 32-bit paging:
 * - Page directory has 1024 entries
 * - Each page table has 1024 entries
 * - Each page is 4096 bytes
 * - One page table maps 1024 * 4096 = 4MB
 */

static u32 page_directory[1024] __attribute__((aligned(4096)));
static u32 first_page_table[1024] __attribute__((aligned(4096)));

static int paging_enabled = 0;

static void load_page_directory(u32* page_directory_addr) {
    __asm__ volatile (
        "mov %0, %%cr3"
        :
        : "r"(page_directory_addr)
    );
}

static void enable_paging(void) {
    u32 cr0;

    __asm__ volatile (
        "mov %%cr0, %0"
        : "=r"(cr0)
    );

    cr0 |= 0x80000000; /* set paging bit */

    __asm__ volatile (
        "mov %0, %%cr0"
        :
        : "r"(cr0)
    );
}

void paging_init(void) {
    /*
     * First, mark every page directory entry as not present.
     * 0x2 = writable, but not present.
     */
    for (u32 i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000002;
    }

    /*
     * Identity-map the first 4MB.
     * This means virtual address == physical address.
     *
     * Example:
     * 0x000B8000 maps to 0x000B8000
     *
     * Important because VGA memory, kernel, and stack are currently all in low memory.
     */
    for (u32 i = 0; i < 1024; i++) {
        first_page_table[i] = (i * 0x1000) | 3;
    }

    /*
     * Put the first page table into the first page directory entry.
     * 3 = present + writable.
     */
    page_directory[0] = ((u32)first_page_table) | 3;

    load_page_directory(page_directory);
    enable_paging();

    paging_enabled = 1;

    console_write("Paging enabled: identity-mapped first 4MB.\n");
}

int paging_is_enabled(void) {
    return paging_enabled;
}