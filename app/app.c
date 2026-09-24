/* app/app.c */
#include "kprintf.h"
#include "task.h"
#include "schedular.h"
#include "app_configs.h"
#include "app.h"

static uint32_t stack_worker1[WORKER_STACK_WORDS];
static uint32_t stack_worker2[WORKER_STACK_WORDS];
static tcb_t    tcb_worker1;
static tcb_t    tcb_worker2;

static void worker_task1(void *arg)
{
    (void)arg;
    while (1) {
        kprintf("[Task 1] Running...\n");
        for (volatile int i = 0; i < 500000; i++);
    }
}

static void worker_task2(void *arg)
{
    (void)arg;
    while (1) {
        kprintf("[Task 2] Running...\n");
        for (volatile int i = 0; i < 500000; i++);
    }
}

/* Entry point launched by the kernel */
void app_main(void *arg)
{
    (void)arg;
    kprintf("[App] Bootstrap task started.\n");

    /* Create application child tasks */
    task_create(&tcb_worker1, stack_worker1, WORKER_STACK_WORDS, worker_task1, (void *)0x1, 1);
    task_create(&tcb_worker2, stack_worker2, WORKER_STACK_WORDS, worker_task2, (void *)0x2, 1);

    scheduler_add(&tcb_worker1);
    scheduler_add(&tcb_worker2);

    kprintf("[App] Worker tasks spawned. Bootstrap transitioning to background...\n");

    while (1) {
        /* Bootstrap task work loop or idle */
        for (volatile int i = 0; i < 1000000; i++);
    }
}