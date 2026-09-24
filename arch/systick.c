#include <stddef.h>
#include "systick.h"
#include "schedular.h"

#define SYST_CSR (*(volatile uint32_t *)0xE000E010u)
#define SYST_RVR (*(volatile uint32_t *)0xE000E014u)
#define SYST_CVR (*(volatile uint32_t *)0xE000E018u)

#define CSR_ENABLE    (1u)
#define CSR_TICKINT   (1u << 1)
#define CSR_CLKSOURCE (1u << 2)

static volatile uint32_t tick_count; // It is shared with ISR and should be volatile

static void systick_disable(void)
{
    SYST_CSR = 0;
}

static void systick_enable(void)
{
    SYST_CSR = CSR_ENABLE | CSR_TICKINT | CSR_CLKSOURCE;
}

void systick_init(uint32_t cycles_per_tick)
{
    systick_disable();              // Disable systick for configuration
    SYST_RVR = cycles_per_tick - 1;
    SYST_CVR = 0;                   // Clear current value
    systick_enable();               // Enable systic
}

// Index 15 exception in vector table
void SysTick_Handler(void)
{
    tick_count++;
    if (current_task != NULL) {       /* only switch once a real task is running */
        scheduler_tick();
    }
}

uint32_t systick_get_ticks(void)
{
    return tick_count;
}