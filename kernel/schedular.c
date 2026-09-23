#include "schedular.h"
#include <stddef.h>

// These two should be visible to PendSV_Handler assembly function
tcb_t *current_task; /* the task whose registers are IN the CPU right now */
tcb_t *next_task;    /* the task the scheduler wants to run next */

tcb_t *ready_list = NULL;   /* circular linked list of READY tasks */

void scheduler_add(tcb_t *t)
{
    if (ready_list == NULL) {
        ready_list = t;
        t->next = t;              /* points to itself: circular list of one */
    } else {
        t->next = ready_list->next; /* Shift and place */
        ready_list->next = t;
    }
}

tcb_t *scheduler_pick_next(void)
{
    ready_list = ready_list->next;   /* rotate: next task in the circle */
    return ready_list;
}