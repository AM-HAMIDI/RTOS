#include "cpu.h"
#include "schedular.h"

/*
 * PendSV_Handler — the context switch itself.
 *
 * Runs at the lowest exception priority, so it only executes once every
 * higher-priority interrupt (SysTick included) has finished. On entry, PSP
 * points at the interrupted task's stack; the CPU has ALREADY pushed
 * R0-R3, R12, LR, PC, xPSR there automatically. This handler only has to
 * save/restore R4-R11 by hand and swap the stack pointer between tasks.
 */

__attribute__((naked)) void PendSV_Handler(void)
{
    __asm volatile (
        /* ---- SAVE current task's context ---- */
        "mrs r0, psp                \n"  /* r0 = current task's stack pointer */
        "stmdb r0!, {r4-r11}        \n"  /* push R4-R11 onto its stack */

        "ldr r1, =current_task      \n"
        "ldr r1, [r1]                \n"  /* r1 = current_task (TCB pointer) */
        "str r0, [r1]                \n"  /* current_task->sp = r0 (sp is offset 0) */

        /* ---- Ask the C scheduler which task runs next ----
           LR currently holds EXC_RETURN, which the final `bx lr` needs.
           `bl` below overwrites LR with a return address inside THIS
           function, so save/restore it around the call. {r3, lr} keeps
           the stack 8-byte aligned across the call, as the AAPCS expects. */
        "push {r3, lr}               \n"
        "bl scheduler_pick_next     \n"  /* r0 = scheduler_pick_next() */
        "pop {r3, lr}                \n"

        "ldr r1, =next_task          \n"
        "str r0, [r1]                \n"  /* next_task = r0 */
        "ldr r1, =current_task      \n"
        "str r0, [r1]                \n"  /* current_task = next_task */

        /* ---- RESTORE next task's context ---- */
        "ldr r0, [r0]                \n"  /* r0 = next_task->sp */
        "ldmia r0!, {r4-r11}        \n"  /* pop R4-R11 from its stack */
        "msr psp, r0                \n"  /* PSP -> base of its hardware frame */

        "bx lr                       \n"  /* exception return: hardware pops
                                              R0-R3/R12/LR/PC/xPSR and resumes it */
    );
}

