#include "console.h"
#include "shell.h"
#include "timer.h"
#include "panic.h"
#include "memory.h"
#include "paging.h"
#include "scheduler.h"
#include "test.h"
#include "fs.h"
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
        console_write("Commands: help, clear, about, ticks, panic, echo, cowsay, mem, alloc, pagefault, multitask, stoptask, tasks, ls, touch, write, cat, rm, runtests\n");
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

else if (str_starts_with(command, "cowsay ")) {
    const char* msg = command + 7;

    console_write(" ----\n< ");
    console_write(msg);
    console_write(" >\n ----\n");

    console_write("        ^__^\n");
    console_write("        (oo)\\_______\n");
    console_write("        (__)\\       )\\/\\\n");
    console_write("            ||----w |\n");
    console_write("            ||     ||\n");
}
else if (str_equal(command, "mem")) {
    console_write("Paging: ");
    console_write(paging_is_enabled() ? "enabled\n" : "disabled\n");

    console_write("Heap start: ");
    console_write_hex(memory_get_heap_start());
    console_write("\n");

    console_write("Heap current: ");
    console_write_hex(memory_get_heap_current());
    console_write("\n");

    console_write("Heap used: ");
    console_write_dec(memory_get_heap_used());
    console_write(" bytes\n");
}
else if (str_starts_with(command, "alloc ")) {
    const char* num = command + 6;
    u32 size = 0;

    while (*num >= '0' && *num <= '9') {
        size = size * 10 + (*num - '0');
        num++;
    }

    void* ptr = kmalloc(size);

    if (ptr == NULL) {
        console_write("Allocation failed.\n");
    } else {
        console_write("Allocated ");
        console_write_dec(size);
        console_write(" bytes at ");
        console_write_hex((u32)ptr);
        console_write("\n");
    }
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
    else if (str_equal(command, "pagefault")) {
    volatile u32* bad_ptr = (u32*)0x00500000;
    *bad_ptr = 1234;
    }
else if (str_equal(command, "runtests")) {
    tests_run_all();
}
else if (str_equal(command, "ls")) {
    fs_list();
}
else if (str_starts_with(command, "touch ")) {
    const char* name = command + 6;

    if (fs_create(name)) {
        console_write("File created.\n");
    } else {
        console_write("Could not create file.\n");
    }
}
else if (str_starts_with(command, "cat ")) {
    const char* name = command + 4;
    const char* data = fs_read(name);

    if (data == NULL) {
        console_write("File not found.\n");
    } else {
        console_write(data);
        console_write("\n");
    }
}
else if (str_starts_with(command, "rm ")) {
    const char* name = command + 3;

    if (fs_delete(name)) {
        console_write("File deleted.\n");
    } else {
        console_write("File not found.\n");
    }
}
else if (str_starts_with(command, "write ")) {
    const char* rest = command + 6;

    char filename[FS_MAX_NAME];
    int i = 0;

    while (*rest && *rest != ' ' && i < FS_MAX_NAME - 1) {
        filename[i++] = *rest++;
    }

    filename[i] = '\0';

    if (*rest == ' ') {
        rest++;
    }

    if (filename[0] == '\0' || rest[0] == '\0') {
        console_write("Usage: write <file> <text>\n");
    } else if (fs_write(filename, rest)) {
        console_write("File written.\n");
    } else {
        console_write("File not found.\n");
    }
}
else if (str_equal(command, "multitask")) {
    task_start_demo();
}
else if (str_equal(command, "stoptask")) {
    task_stop_demo();
}
else if (str_equal(command, "tasks")) {
    task_list();
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