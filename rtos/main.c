volatile int g_data = 0x1234;   /* .data: needs the flash -> RAM copy */
volatile int g_bss;             /* .bss: must end up zero */

#include <stdint.h>
#include "kprintf.h"
#include "systick.h"

#define TICK_RATE       1000u         // 1000 HZ tick rate
#define CPU_CLK_RATE    2000000000u   // 2GHZ cpu clock rate

int main(void)
{
    kprintf("\n=== RTOS Phase 0 ===\n");
    kprintf("g_data = 0x%x (expect 0x1234)\n", (uint32_t)g_data);
    kprintf("g_bss  = %u   (expect 0)\n",      (uint32_t)g_bss);


    while (1) {
        __asm volatile ("wfi");    /* sleep until an interrupt */
    }
}