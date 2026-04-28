#include "scheduler.h"
#include "console.h"

#define MAX_TASKS 3

typedef void (*task_func_t)(void);

typedef struct {
    int id;
    const char* name;
    task_func_t function;
    u32 counter;
    int active;
} task_t;

static task_t tasks[MAX_TASKS];
static int current_task = 0;
static int multitasking_enabled = 0;

static void task_a(void) {
    tasks[0].counter++;
}

static void task_b(void) {
    tasks[1].counter++;
}

static void task_c(void) {
    tasks[2].counter++;
}

void task_init(void) {
    tasks[0].id = 0;
    tasks[0].name = "task_a";
    tasks[0].function = task_a;
    tasks[0].counter = 0;
    tasks[0].active = 1;

    tasks[1].id = 1;
    tasks[1].name = "task_b";
    tasks[1].function = task_b;
    tasks[1].counter = 0;
    tasks[1].active = 1;

    tasks[2].id = 2;
    tasks[2].name = "task_c";
    tasks[2].function = task_c;
    tasks[2].counter = 0;
    tasks[2].active = 1;

    multitasking_enabled = 0;
}

void task_start_demo(void) {
    multitasking_enabled = 1;
    console_write("Multitasking demo started.\n");
}

void task_stop_demo(void) {
    multitasking_enabled = 0;
    console_write("Multitasking demo stopped.\n");
}

void scheduler_tick(void) {
    if (!multitasking_enabled) {
        return;
    }

    for (int tries = 0; tries < MAX_TASKS; tries++) {
        current_task++;

        if (current_task >= MAX_TASKS) {
            current_task = 0;
        }

        if (tasks[current_task].active && tasks[current_task].function) {
            tasks[current_task].function();
            return;
        }
    }
}

void task_list(void) {
    console_write("ID   Name      Counter\n");

    for (int i = 0; i < MAX_TASKS; i++) {
        console_write_dec(tasks[i].id);
        console_write("    ");
        console_write(tasks[i].name);
        console_write("    ");
        console_write_dec(tasks[i].counter);
        console_write("\n");
    }
}