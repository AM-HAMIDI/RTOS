#include <stddef.h>
#include "schedular.h"
#include "cpu.h"

// These two should be visible to PendSV_Handler assembly function
tcb_t *current_task; /* the task whose registers are IN the CPU right now */
tcb_t *next_task;    /* the task the scheduler wants to run next */

tcb_t *ready_list = NULL;           /* circular linked list of READY tasks */

void scheduler_start(void)
{
    /* 1. Ensure current_task points to the initial task */
    current_task = ready_list;

    /* 2. Configure PendSV to the lowest priority */
    arch_set_pendsv_priority();

    /* 3. Configure and start the SysTick timer (e.g., 1ms tick) */
    systick_init(1000);

    /* 4. Trigger SVC 0 to enter SVC_Handler and start the first task */
    __asm volatile ("cpsie i" : : : "memory");
    __asm volatile ("svc 0");

    /* Never reached */
    while (1);
}

void scheduler_add(tcb_t *t)
{
    if (ready_list == NULL) {
        ready_list = t;
        t->next = t;                /* points to itself: circular list of one */
    } else {
        t->next = ready_list->next; /* Shift and place */
        ready_list->next = t;
    }
}

tcb_t *scheduler_pick_next(void)
{
    ready_list = ready_list->next;  /* rotate: next task in the circle */
    return ready_list;
}

void scheduler_tick(void)
{
    arch_trigger_context_switch();
}