volatile int g_data = 0x1234;   /* .data: needs the flash -> RAM copy */
volatile int g_bss;             /* .bss: must end up zero */

#include <stdint.h>
#include "kprintf.h"
#include "systick.h"

#define CYCLES_PER_TICK  1000000u
#define PRINT_EVERY_TICK 10u

int main(void)
{
    kprintf("\n=== RTOS Phase 0 ===\n");
    kprintf("g_data = 0x%x (expect 0x1234)\n", (uint32_t)g_data);
    kprintf("g_bss  = %u   (expect 0)\n",      (uint32_t)g_bss);

    // Setup systick
    systick_init(CYCLES_PER_TICK);
    kprintf("SysTick started\n");

    uint32_t last = 0;
    while (1) {
        uint32_t now = systick_get_ticks();
        if (now - last >= PRINT_EVERY_TICK) {
            last = now;
            kprintf("tick=%u\n", now);
        }
        __asm volatile ("wfi");      /* sleep until the next interrupt */
    }
}