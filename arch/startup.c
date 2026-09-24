#include <stdint.h>

#define INF_LOOP while(1){}

// ARM uses 32-bit words for addressing
extern uint32_t _estack, _sidata, _sdata, _edata, _sbss, _ebss;

// Main program of RTOS
int main(void);

// The main Reset_handler sits at 0x00000004
void Reset_Handler(void);

// Define inifinite loop for handlers until they implement correctly 
void Default_Handler(void);

// Define weak aliases for different exception handlers
void NMI_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)    __attribute__((weak, alias("Default_Handler")));

typedef void (*isr_t)(void);

// Main vector table sits at 0x00000000
__attribute__((section(".isr_vector"), used))
const isr_t vector_table[] = {
    (isr_t)&_estack,     /* 0: initial stack pointer */
    Reset_Handler,       /* 1: reset */
    NMI_Handler,         /* 2 */
    HardFault_Handler,   /* 3 */
    MemManage_Handler,   /* 4 */
    BusFault_Handler,    /* 5 */
    UsageFault_Handler,  /* 6 */
    0, 0, 0, 0,          /* 7-10: reserved */
    SVC_Handler,         /* 11 */
    DebugMon_Handler,    /* 12 */
    0,                   /* 13: reserved */
    PendSV_Handler,      /* 14 */
    SysTick_Handler,     /* 15 */
    /* 16+: external IRQs, added later when we need them */
};

// Index 1 exception in vector table
void Reset_Handler(void)
{
    uint32_t *src = &_sidata;
    uint32_t *des = &_sdata;

    // Copy flash memory data to .data section
    while(des < &_edata)
    {
        *des++ = *src++;
    }

    // Zero .bss section
    for(des = &_sbss ; des <&_ebss ; des++)
    {
        *des = 0;
    }

    // Call main program 
    main();

    // Do while loop to stuck here and never return to flash blocks
    INF_LOOP
}

void Default_Handler(void)
{
    INF_LOOP
}