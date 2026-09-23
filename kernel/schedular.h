#ifndef SCHEDULAR_H
#define SCHEDULAR_H

#include "task.h"

#define SCB_ICSR       (*(volatile uint32_t *)0xE000ED04u)
#define ICSR_PENDSVSET (1u << 28) // Sets PendSV exception to pending and request context switch.
#define ICSR_PENDSVCLR (1u << 27) // Clears the pending status of PendSV
#define SCB_SHPR3    (*(volatile uint32_t *)0xE000ED20u)

void scheduler_add(tcb_t *t);
tcb_t *scheduler_pick_next(void);

static inline void trigger_context_switch(void)
{
    SCB_ICSR = ICSR_PENDSVSET;
}

static inline void clean_PendSV(void)
{
    SCB_ICSR = ICSR_PENDSVCLR;
}

void set_pendsv_lowest_priority(void)
{
    SCB_SHPR3 |= (0xFFu << 16);   /* PendSV priority field = byte 2 of SHPR3, set to lowest (0xFF) */
}

#endif