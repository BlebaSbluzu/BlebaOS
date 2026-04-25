#ifndef CONSOLE_H
#define CONSOLE_H

#include "common.h"
void console_init(void);
void console_clear(void);
void console_putc(char c);
void console_write(const char* str);
void console_write_hex(u32 value);
void console_write_dec(u32 value);

#endif