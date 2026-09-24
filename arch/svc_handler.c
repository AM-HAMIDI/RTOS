#include <stdint.h>
#include "schedular.h"

// Index 11 exception in vector table
__attribute__((naked)) void SVC_Handler(void)
{
    __asm volatile (
        /* r0 = &current_task */
        "ldr r0, =current_task          \n"
        "ldr r0, [r0]                   \n" /* r0 = current_task (tcb_t*) */
        "ldr r0, [r0]                   \n" /* r0 = current_task->sp */

        /* Pop R4-R11 manually */
        "ldmia r0!, {r4-r11}            \n"

        /* Point PSP to the base of the hardware frame */
        "msr psp, r0                    \n"

        /* Clear MSP/CONTROL configuration to use PSP in Thread Mode */
        "mov r0, #2                     \n"
        "msr CONTROL, r0                \n"
        "isb                            \n"

        /* Return using EXC_RETURN: Thread Mode, use PSP */
        "ldr lr, =0xFFFFFFFD            \n"
        "bx lr                          \n"
    );
}