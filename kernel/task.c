#include <stddef.h>
#include "task.h"

#define INIT_XPSR         0x01000000u // Thumb bit (bit 24)

void task_create(tcb_t *tcb, uint32_t *stack_base, uint32_t stack_words,
                  task_func_t entry, void *arg, uint32_t priority)
{
    uint32_t *sp = stack_base + stack_words;

    /* ---- hardware-saved frame (CPU pops this on exception return) ---- */
    *(--sp) = INIT_XPSR;                    /* xPSR = Stataus register*/
    *(--sp) = (uint32_t)entry;              /* PC   = task entry point */
    *(--sp) = (uint32_t) task_exit_handler; /* LR   = Exit handler*/
    *(--sp) = 0;                            /* R12 */
    *(--sp) = 0;                            /* R3  */
    *(--sp) = 0;                            /* R2  */
    *(--sp) = 0;                            /* R1  */
    *(--sp) = (uint32_t)arg;                /* R0  = task argument */

    /* ---- software-saved frame (PendSV pushes/pops this) ---- */
    *(--sp) = 0;                            /* R11 */
    *(--sp) = 0;                            /* R10 */
    *(--sp) = 0;                            /* R9  */
    *(--sp) = 0;                            /* R8  */
    *(--sp) = 0;                            /* R7  */
    *(--sp) = 0;                            /* R6  */
    *(--sp) = 0;                            /* R5  */
    *(--sp) = 0;                            /* R4  */

    tcb->sp = sp;
    tcb->state = TASK_READY;
    tcb->priority = priority;
    tcb->delay_ticks = 0;
    tcb->next = NULL;
}

void task_exit_handler(void)
{
    while (1) {
        __asm volatile ("wfi");
    }
}