#include "common.h"
#include "irq.h"
#include "shell.h"

static const char keyboard_map[128] = {
    0,
    27,
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    '\b',
    '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    '\n',
    0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0,
    '*',
    0,
    ' ',
    0,
    0,0,0,0,0,0,0,0,0,0,
    0,
    0,
    0,0,0,0,0,0,0,
    0,0,
    0,
    0,
    0,0,0,0,
    0,0,0,
    0,
    0,0,0,
    0,0,0,0,0
};

static void keyboard_callback(void) {

    // data buffer for the ps/2 keyboard: 
    u8 scancode = inb(0x60);


    //for released keybs to prevent repeatedly writing a character
    if (scancode & 0x80) {
        return;
    }

    if (scancode >= 128) {
        return;
    }

    char c = keyboard_map[scancode];
    if (c) {
        shell_handle_input(c);
    }
}

void keyboard_init(void) {
    irq_install_handler(1, keyboard_callback);
}