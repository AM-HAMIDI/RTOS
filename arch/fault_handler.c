#include <stdint.h>
#include "kprintf.h"

// We will use hardfault handler for debugging for different faults of system
void HardFault_Handler(void);
void HardFault_Handler_C(uint32_t *stacked_regs);

// Index 3 exception in vector table
__attribute__((naked)) void HardFault_Handler(void)
{
    __asm volatile (
        "tst lr, #4          \n"   /* bit 2 of EXC_RETURN: 0 = MSP, 1 = PSP */
        "ite eq              \n"
        "mrseq r0, msp        \n"
        "mrsne r0, psp        \n"
        "b HardFault_Handler_C \n"
    );
}

void HardFault_Handler_C(uint32_t *stacked_regs)
{
    // R0 holds the stack pointer value that points at the automatically-pushed
    // fault stack frame (R0, R1, R2, R3, R12, LR, PC, xPSR of the faulting context).
    uint32_t r0   = stacked_regs[0];
    uint32_t r1   = stacked_regs[1];
    uint32_t r2   = stacked_regs[2];
    uint32_t r3   = stacked_regs[3];
    uint32_t r12  = stacked_regs[4];
    uint32_t lr   = stacked_regs[5];
    uint32_t pc   = stacked_regs[6];
    uint32_t xpsr = stacked_regs[7];

    kprintf("\n*** HARD FAULT ***\n");
    kprintf("PC=0x%x LR=0x%x XPSR=0x%x\n", pc, lr, xpsr);
    kprintf("R0=0x%x R1=0x%x R2=0x%x R3=0x%x R12=0x%x\n", r0, r1, r2, r3, r12);

    while (1) { }
}
