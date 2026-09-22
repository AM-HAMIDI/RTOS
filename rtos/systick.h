#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

/* 
    Definition : 
    The SysTick timer is the heartbeat of your RTOS.
    It is a 24-bit hardware timer built directly into the ARM Cortex-M processor core.
    It operates as a simple countdown timer: it starts at a specific value, counts down to zero,
    triggers a hardware interrupt (the SysTick exception), immediately reloads its starting value,
    and repeats forever.

    Registers : 
    These three memory-mapped registers control exactly how the timer behaves:
    
    1 - CSR (Control and Status Register - 0xE000E010): The command center.
        Bit 0 (ENABLE): Turns the timer on (1) or off (0).
        Bit 1 (TICKINT): When set to 1, the timer will fire an exception (interrupt)
                        every time it hits zero. If set to 0, it just counts silently and
                        you have to manually poll it. Your RTOS needs this set to 1.
        Bit 2 (CLKSOURCE): When set to 1, the timer ticks at the exact speed of
        your main CPU clock.

    2 - RVR (Reload Value Register : 0xE000E014): This holds the starting number for the countdown.
    Because it is 24-bit, the maximum value you can load is 0xFFFFFF (16,777,215).

    3 - CVR (Current Value Register - 0xE000E018): This holds the live countdown value.
        If you read it, you see exactly where the timer is. As a safety feature,
        writing any value to this register instantly resets it to 0 and clears any pending
        interrupts, giving you a clean slate.
*/

void systick_init(uint32_t cycles_per_tick);
uint32_t systick_get_ticks(void);

#endif