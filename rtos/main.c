volatile int g_data = 0x1234;   /* .data: needs the flash -> RAM copy */
volatile int g_bss;             /* .bss: must end up zero */

#include "uart.h"

int main(void)
{
    uart_puts("Hello from bare-metal Cortex-M3!\n");

    while (1) {
        __asm volatile ("wfi");    /* sleep until an interrupt */
    }
}