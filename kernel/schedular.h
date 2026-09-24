#ifndef SCHEDULAR_H
#define SCHEDULAR_H

#include "task.h"

extern tcb_t *current_task;
extern tcb_t *next_task;

void schedular_start(void);

void   scheduler_add(tcb_t *t);
tcb_t *scheduler_pick_next(void);

void scheduler_tick(void);

#endif