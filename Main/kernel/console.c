#include "common.h"
#include "console.h"

static volatile u16* const VGA_MEMORY = (u16*)0xB8000;
static const u8 VGA_WIDTH = 80;
static const u8 VGA_HEIGHT = 25;

static u8 console_row = 0;
static u8 console_col = 0;
static u8 console_color = 0x07; 

static u16 vga_entry(unsigned char c, u8 color) {
    return (u16)color << 8 | (u16)c;
}
void console_update_cursor(void) {
    u16 pos = console_row * VGA_WIDTH + console_col;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (u8)(pos & 0xFF));

    outb(0x3D4, 0x0E);
    outb(0x3D5, (u8)((pos >> 8) & 0xFF));
}
static void console_newline(void) {
    console_col = 0;

    if (console_row < VGA_HEIGHT - 1) {
        console_row++;
    } else {
        // scroll everything up
        for (u32 y = 1; y < VGA_HEIGHT; y++) {
            for (u32 x = 0; x < VGA_WIDTH; x++) {
                VGA_MEMORY[(y - 1) * VGA_WIDTH + x] =
                    VGA_MEMORY[y * VGA_WIDTH + x];
            }
        }

        // clear last line
        for (u32 x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + x] =
                vga_entry(' ', console_color);
        }
    }

    console_update_cursor();
}

static void console_backspace(void) {
    /* if already at top-left, do nothing */
    if (console_row == 0 && console_col == 0) {
        console_update_cursor();

        return;
    }

    /* move left if possible */
    if (console_col > 0) {
        console_col--;
    } else {
        /* move to end of previous line */
        console_row--;
        console_col = VGA_WIDTH - 1;
    }

    /* erase character at current cursor position */
    VGA_MEMORY[console_row * VGA_WIDTH + console_col] = vga_entry(' ', console_color);
}
void console_set_color(u8 color) {
    console_color = color;
}
void console_clear(void) {
    for (u32 y = 0; y < VGA_HEIGHT; y++) {
        for (u32 x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[y * VGA_WIDTH + x] = vga_entry(' ', console_color);
        }
    }

    console_row = 0;
    console_col = 0;
        console_update_cursor();

}

void console_putc(char c) {
    if (c == '\n') {
        console_newline();
        return;
    }

    if (c == '\r') {
        console_col = 0;
        console_update_cursor();
        return;
    }

    if (c == '\b') {
        console_backspace();
        console_update_cursor();
        return;
    }

    VGA_MEMORY[console_row * VGA_WIDTH + console_col] =
        vga_entry((unsigned char)c, console_color);

    console_col++;

    if (console_col >= VGA_WIDTH) {
        console_newline();
    } else {
        console_update_cursor();
    }
}

void console_write(const char* str) {
    while (*str) {
        console_putc(*str);
        str++;

    }
}

void console_write_hex(u32 value) {
    const char* digits = "0123456789ABCDEF";
    console_write("0x");

    for (int i = 7; i >= 0; i--) {
        u8 nibble = (value >> (i * 4)) & 0xF;
        console_putc(digits[nibble]);
    }
}



void console_write_dec(u32 value) {
    char buffer[16];
    int i = 0;

    if (value == 0) {
        console_putc('0');
        return;
    }

    while (value > 0) {
        buffer[i++] = '0' + (value % 10);
        value = value / 10;
    }

    while (i > 0) {
        console_putc(buffer[--i]);
    }
}

void console_init(void) {
    console_clear();
    console_set_color(0x0A); // light green on black
    console_write("================================\n");
    console_write("          BlebaOS v0.1\n");
    console_write("================================\n");
    console_set_color(0x07); // normal grey
    console_update_cursor();

}