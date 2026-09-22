#include <stddef.h>
#include "task.h"

#define INIT_XPSR         0x01000000u // Thumb bit
#define MOCK_EXIT_HANDLER 0xFFFFFFFFu // Todo : should change to real exit handler address

void task_create(tcb_t *tcb, uint32_t *stack_base, uint32_t stack_words,
                  task_func_t entry, void *arg, uint32_t priority)
{
    uint32_t *sp = stack_base + stack_words;

    *(--sp) = INIT_XPSR;         /* xPSR = Stataus register*/
    *(--sp) = (uint32_t)entry;   /* PC   = task entry point */
    *(--sp) = MOCK_EXIT_HANDLER; /* LR   = Exit handler*/
    *(--sp) = 0;                 /* R12 */
    *(--sp) = 0;                 /* R3  */
    *(--sp) = 0;                 /* R2  */
    *(--sp) = 0;                 /* R1  */
    *(--sp) = (uint32_t)arg;     /* R0  = task argument */
    *(--sp) = 0;                 /* R11 */
    *(--sp) = 0;                 /* R10 */
    *(--sp) = 0;                 /* R9  */
    *(--sp) = 0;                 /* R8  */
    *(--sp) = 0;                 /* R7  */
    *(--sp) = 0;                 /* R6  */
    *(--sp) = 0;                 /* R5  */
    *(--sp) = 0;                 /* R4  */

    tcb->sp = sp;
    tcb->state = TASK_READY;
    tcb->priority = priority;
    tcb->delay_ticks = 0;
    tcb->next = NULL;
}

void task_exit_handler(void)
{

}

void task_delete(void)
{

}