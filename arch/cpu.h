#ifndef CPU_H
#define CPU_H

#include <stdint.h>

/* ---- SCB: System Control Block ---- */
// 0xE000E000 Region
#define SCB_ICSR       (*(volatile uint32_t *)0xE000ED04u)
#define SCB_SHPR3      (*(volatile uint32_t *)0xE000ED20u)

#define ICSR_PENDSVSET (1u << 28)   /* write 1: request a PendSV */
#define ICSR_PENDSVCLR (1u << 27)   /* write 1: cancel a pending (not-yet-run) PendSV */

/* Request a context switch. PendSV fires once nothing higher-priority is running. */
static inline void arch_trigger_context_switch(void)
{
    SCB_ICSR = ICSR_PENDSVSET;
}

/* Cancel a pending PendSV before it runs. */
static inline void arch_clear_pending_context_switch(void)
{
    SCB_ICSR = ICSR_PENDSVCLR;
}

/* PendSV must sit at the LOWEST priority so it never preempts a real interrupt
   mid-flight — it always waits until every other handler has finished. */
static inline void arch_set_pendsv_priority(void)
{
    SCB_SHPR3 |= (0xFFu << 16);   /* PendSV priority = byte 2 of SHPR3 */
}

#endif