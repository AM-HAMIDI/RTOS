#ifndef TASK_H
#define TASK_H

#include <stdint.h>

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_SLEEPING
} task_state_t;

typedef enum {
    CRITICAL,
    HIGH,
    MID,
    LOW
} task_priority_t;

typedef struct tcb {
    uint32_t        *sp;           // PendSV asm will access this as index 0
    task_state_t    state;
    task_priority_t priority;
    uint32_t        delay_ticks;
    struct tcb      *next;         // For creating linked_list (round robin)
} tcb_t;

typedef void (*task_func_t)(void *arg);

void task_create(tcb_t *tcb, uint32_t *stack_base, uint32_t stack_words,
                  task_func_t entry, void *arg, task_priority_t priority);
    
void task_exit_handler(void);
void task_delete(void);

#endif