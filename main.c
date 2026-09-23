#include <stdint.h>
#include "kprintf.h"
#include "systick.h"
#include "schedular.h"
#include "task.h"
#include "cpu.h"

volatile int g_data = 0x1234;
volatile int g_bss;

#define HARD_CRASH       __asm volatile ("udf #0")
#define CYCLES_PER_TICK  1000000u
#define PRINT_EVERY_TICK 10u
#define STACK_WORDS       64u

static uint32_t stack_a[STACK_WORDS];
static uint32_t stack_b[STACK_WORDS];
static uint32_t stack_c[STACK_WORDS];
static tcb_t    tcb_a, tcb_b, tcb_c;

void dummy_task(void *arg) { (void)arg; while (1) {} }

int main(void)
{
    kprintf("\n=== Phase 0 checks ===\n");
    kprintf("g_data = 0x%x (expect 0x1234)\n", (uint32_t)g_data);
    kprintf("g_bss  = %u   (expect 0)\n", (uint32_t)g_bss);

    arch_set_pendsv_priority();
    systick_init(CYCLES_PER_TICK);
    kprintf("SysTick started\n");

    kprintf("\n=== Phase 1: scheduler round-robin test ===\n");
    task_create(&tcb_a, stack_a, STACK_WORDS, dummy_task, (void *)0xA, 1);
    task_create(&tcb_b, stack_b, STACK_WORDS, dummy_task, (void *)0xB, 1);
    task_create(&tcb_c, stack_c, STACK_WORDS, dummy_task, (void *)0xC, 1);

    scheduler_add(&tcb_a);
    scheduler_add(&tcb_b);
    scheduler_add(&tcb_c);

    for (int i = 0; i < 7; i++) {
        tcb_t *t = scheduler_pick_next();
        const char *name = (t == &tcb_a) ? "A" : (t == &tcb_b) ? "B" : "C";
        kprintf("pick_next() -> Task %s (sp=0x%x)\n", name, (uint32_t)t->sp);
    }
    /* kernel_start() doesn't exist yet -- tasks aren't actually running.
       This only proves the ready list and picker are correct. */

    uint32_t last = 0;
    while (1) {
        uint32_t now = systick_get_ticks();
        if (now - last >= PRINT_EVERY_TICK) {
            last = now;
            kprintf("tick=%u\n", now);
        }
        __asm volatile ("wfi");
    }
}