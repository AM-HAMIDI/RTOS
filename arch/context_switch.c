#include "schedular.h"

__attribute__((naked)) void PendSV_Handler(void)
{
    __asm volatile (
        /* ---- SAVE current task's context ---- */
        "mrs r0, psp                \n"   /* r0 = current PSP (task's stack pointer) */
        "stmdb r0!, {r4-r11}        \n"   /* push R4-R11 onto THAT stack, r0 auto-decrements */

        "ldr r1, =current_task      \n"   /* r1 = &current_task (address of the pointer) */
        "ldr r1, [r1]                \n"   /* r1 = current_task  (the actual TCB pointer) */
        "str r0, [r1]                \n"   /* current_task->sp = r0  (sp is 1st struct member) */

        /* ---- PICK next task ---- */
        "bl scheduler_pick_next     \n"   /* r0 = scheduler_pick_next() return value */
        "ldr r1, =next_task          \n"
        "str r0, [r1]                \n"   /* next_task = r0 */

        "ldr r1, =current_task      \n"
        "str r0, [r1]                \n"   /* current_task = next_task (r0) */

        /* ---- RESTORE next task's context ---- */
        "ldr r0, [r0]                \n"   /* r0 = next_task->sp (deref: sp is 1st member) */
        "ldmia r0!, {r4-r11}        \n"   /* pop R4-R11 back from ITS stack, r0 auto-increments */
        "msr psp, r0                \n"   /* PSP now points at that task's hardware frame */

        "bx lr                       \n"   /* exception return — hardware pops R0-R3/R12/LR/PC/xPSR */
    );
}

