#include "console.h"
#include "shell.h"
#include "timer.h"
#include "panic.h"

static char shell_buffer[128];
static int shell_length = 0;

static int str_equal(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == *b;
}

static int str_starts_with(const char* str, const char* prefix) {
    while (*prefix) {
        if (*str != *prefix) {
            return 0;
        }
        str++;
        prefix++;
    }
    return 1;
}

static void shell_execute(const char* command) {
    if (str_equal(command, "help")) {
        console_write("Commands: help, clear, about, ticks, panic, echo, cowsay\n");
    }
    else if (str_equal(command, "clear")) {
        console_clear();
    }
    else if (str_equal(command, "cowsay")) {
    console_write("        ^__^\n");
    console_write("        (oo)\\_______\n");
    console_write("        (__)\\       )\\/\\\n");
    console_write("            ||----w |\n");
    console_write("            ||     ||\n");
}
    else if (str_equal(command, "about")) {
        console_write("BlebaOS Stage 4 shell\n");
    }
    else if (str_equal(command, "ticks")) {
        console_write("Ticks: ");
        console_write_dec(timer_get_ticks());
        console_write("\n");
    }
    else if (str_equal(command, "panic")) {
        panic("Manual panic command invoked");
    }
    else if (str_starts_with(command, "echo ")) {
        console_write(command + 5);
        console_write("\n");
    }
    else if (command[0] == '\0') {
        /* do nothing */
    }
    else {
        console_write("Unknown command: ");
        console_write(command);
        console_write("\n");
    }
}

void shell_prompt(void) {
    console_write("> ");
}

void shell_init(void) {
    shell_length = 0;
    shell_prompt();
}

void shell_handle_input(char c) {
    if (c == '\n') {
        console_write("\n");
        shell_buffer[shell_length] = '\0';
        shell_execute(shell_buffer);
        shell_length = 0;
        shell_prompt();
        return;
    }

    if (c == '\b') {
        if (shell_length > 0) {
            shell_length--;
            console_putc('\b');
            console_putc(' ');
            console_putc('\b');
        }
        return;
    }

    if (shell_length < (int)(sizeof(shell_buffer) - 1)) {
        shell_buffer[shell_length++] = c;
        console_putc(c);
    }
}