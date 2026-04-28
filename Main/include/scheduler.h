#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "common.h"


void task_init(void);
void scheduler_tick(void);

void task_start_demo(void);
void task_stop_demo(void);
void task_list(void);


#endif