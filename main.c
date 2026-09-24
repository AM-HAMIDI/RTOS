#include <stdint.h>
#include "kprintf.h"
#include "systick.h"
#include "schedular.h"
#include "task.h"
#include "cpu.h"
#include "app/app.h"

#define CYCLES_PER_TICK   1000000u
#define BOOT_STACK_WORDS  256u

static uint32_t boot_stack[BOOT_STACK_WORDS];
static tcb_t    boot_tcb;

int main(void)
{
    kprintf("\n=== Booting RTOS Kernel ===\n");

    /* 1. Hardware timer initialization */
    systick_init(CYCLES_PER_TICK);

    /* 2. Create the bootstrap application task */
    task_create(&boot_tcb, boot_stack, BOOT_STACK_WORDS, app_main, (void *)0, 1);
    scheduler_add(&boot_tcb);

    kprintf("Starting scheduler...\n");

    /* 3. Hand control over to tasks (does not return) */
    scheduler_start();

    /* Unreachable */
    while (1) {}
}