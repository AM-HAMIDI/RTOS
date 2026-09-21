# Thermal-Aware RTOS Kernel for Cortex-M3

A preemptive real-time operating system kernel written from scratch in C for ARM Cortex-M3, running under QEMU, with a built-in thermal and power simulation used to compare thermal-management scheduling policies.

> **Status:** Phase 0 (bare-metal foundation) in progress. Startup code, linker script and UART "Hello" are done. `kprintf`, SysTick and the GDB workflow are next.

---

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [Development Environment](#2-development-environment)
3. [Roadmap at a Glance](#3-roadmap-at-a-glance)
4. [Repository Layout](#4-repository-layout)
5. [Phase 0: Bare-Metal Foundation (detailed)](#5-phase-0-bare-metal-foundation)
6. [Phase 1: Kernel Core](#6-phase-1-kernel-core)
7. [Phase 2: Synchronization Primitives](#7-phase-2-synchronization-primitives)
8. [Phase 3: Real-Time Scheduling](#8-phase-3-real-time-scheduling)
9. [Phase 4: Thermal and Power Model](#9-phase-4-thermal-and-power-model)
10. [Phase 5: Thermal-Aware Policies and Experiments](#10-phase-5-thermal-aware-policies-and-experiments)
11. [Phase 6: Polish, Testing, CI, Documentation](#11-phase-6-polish-testing-ci-documentation)
12. [Methodology and Limitations](#12-methodology-and-limitations)
13. [Timeline](#13-timeline)
14. [Progress Checklist](#14-progress-checklist)
15. [Resume Bullets](#15-resume-bullets)

---

## 1. Project Overview

### Goal

Build a preemptive RTOS kernel for Cortex-M3 that:

- schedules periodic real-time tasks with **RMS** and **EDF**,
- simulates a **thermal and power model** (RC thermal network plus DVFS),
- compares **thermal-management policies** on measurable metrics: peak temperature, time above threshold, total energy and deadline misses.

### Why this project

| Aspect | Value |
|---|---|
| Kernel written from scratch | Shows depth: assembly context switch, stack frames, exception handling. Not "I used FreeRTOS". |
| Thermal/energy-aware scheduling | Connects to my thermal-management research, so the project has a research story and not only an engineering one. |
| POS-flavored workloads | Ties to my day job as an embedded developer on POS devices (crypto bursts, thermal printer head, card-reader polling, display refresh). |
| Laptop-only | Everything runs in QEMU, so no hardware is required. |

### Scope

This is **one coherent project** that combines three ideas:

1. A **thermal-aware scheduler** (the research angle),
2. An **RTOS kernel from scratch** (the systems angle),
3. A **HAL, drivers and tests** with CI (the engineering-quality angle).

Embedded Linux (Buildroot/Yocto on QEMU) is intentionally left out and can be a separate small project later.

---

## 2. Development Environment

### Host

Ubuntu on a laptop. No development board.

### Tools

| Tool | Purpose |
|---|---|
| `qemu-system-arm` | Emulates the Cortex-M3 board (`lm3s6965evb`) |
| `arm-none-eabi-gcc` | Cross compiler for Cortex-M |
| `gdb-multiarch` | Debugger that attaches to QEMU |
| `make` | Build system |
| Python + matplotlib | Analysis tools and result plots (later phases) |

### Building QEMU from source (ARM only)

```bash
wget https://download.qemu.org/qemu-11.1.1.tar.xz
tar xvJf qemu-11.1.1.tar.xz
cd qemu-11.1.1

sudo apt install -y build-essential ninja-build pkg-config python3 python3-venv \
    libglib2.0-dev libpixman-1-dev libslirp-dev git flex bison

./configure --target-list=arm-softmmu     # ARM system emulation only, much faster build
make -j$(nproc)                           # use -j4 if the laptop overheats
sudo make install                         # puts qemu-system-arm on PATH
```

Notes:

- `KVM/HVF/WHPX: NO` in the configure summary is normal. Hardware virtualization only accelerates guests with the same architecture as the host.
- `TCG support: YES` is what matters. TCG is QEMU's software translator, which is how an x86 machine emulates ARM.
- **Common error:** `make: qemu-system-arm: No such file or directory`. The binary is in `qemu-11.1.1/build/` but not on `PATH`. Fix with `sudo make install`, or run `make run QEMU=/path/to/qemu-11.1.1/build/qemu-system-arm`. The alternative is `sudo apt install qemu-system-arm`, which is also fine for this project.

### Toolchain and debugger

```bash
sudo apt install -y gcc-arm-none-eabi binutils-arm-none-eabi gdb-multiarch
```

### Verify

```bash
qemu-system-arm --version
qemu-system-arm -M help | grep -Ei "lm3s6965evb|mps2-an385"
arm-none-eabi-gcc --version
```

### Target machine

| Property | Value |
|---|---|
| Board | `lm3s6965evb` (Stellaris, Cortex-M3) |
| Flash | 256 KB at `0x00000000` |
| SRAM | 64 KB at `0x20000000` |
| UART0 | `0x4000C000` |
| SysTick | `0xE000E010` to `0xE000E018` (core peripheral) |

---

## 3. Roadmap at a Glance

| Phase | Weeks* | Focus | Est. hours | Status |
|---|---|---|---|---|
| 0 | 1 | Bare-metal foundation | 10-12 | In progress |
| 1 | 2-3 | Kernel core | 20-25 | Planned |
| 2 | 4 | Synchronization primitives | 15 | Planned |
| 3 | 5-6 | Real-time schedulers and analysis | 20 | Planned |
| 4 | 7-8 | Thermal and power model | 20 | Planned |
| 5 | 9-10 | Thermal-aware policies and experiments | 25 | Planned |
| 6 | 11-12 | Drivers, tests, CI, documentation | 20 | Planned |

\*Assuming about 10-12 hours per week.

The **MVP** is phases 0 to 3 plus a basic thermal model, and it is already a solid resume project.

---

## 4. Repository Layout

### Current (Phase 0, flat directory `rtos/`)

```
rtos/
├── linker.ld        memory map and section placement           (done)
├── startup.c        vector table + Reset_Handler               (done)
├── uart.h / uart.c  UART0 driver                               (done)
├── main.c           entry point                                (in progress)
├── kprintf.h / .c   minimal printf                             (next)
├── systick.h / .c   SysTick timer + tick counter               (next)
└── Makefile
```

### Target layout (reorganized as the project grows)

```
/kernel     scheduler, tasks, synchronization, timers
/arch       Cortex-M port: startup, PendSV, SVC, linker script
/hal        uart, timer, temperature sensor
/thermal    RC model, DVFS, policies
/apps       workloads and experiments
/tools      Python: schedulability analysis, plots
/tests      host tests + QEMU tests
```

---

## 5. Phase 0: Bare-Metal Foundation

**Purpose:** get my own code running on the (emulated) CPU with no OS, no libc and no vendor HAL, and make the system observable (UART output, SysTick heartbeat, GDB debugging). Everything in later phases sits on top of this.

### 5.1 What happens at power-on

The very first thing is **not** `main()` and not even my code. The CPU hardware reads two 32-bit words from address `0x00000000`:

```
Power-on / reset                        (hardware acts, none of my code has run yet)
   │
   ▼
[1] CPU reads word 0 at 0x00000000  →  loads it into SP   (0x20010000, top of RAM)
[2] CPU reads word 1 at 0x00000004  →  loads it into PC   (address of Reset_Handler)
   │
   ▼                                    (my code starts here)
[3] Reset_Handler: copy .data  FLASH → RAM
[4] Reset_Handler: zero .bss
[5] (later: clock/hardware init goes here)
[6] Reset_Handler calls main()
[7] main() never returns   (while(1) after the call is a safety net)
```

Key points:

- The stack pointer is valid **before** any code runs, which is why `Reset_Handler` can be written in C (C needs a stack for locals).
- The reset handler address is **odd** (bit 0 = 1). That is the Thumb flag. Cortex-M only runs Thumb code, and the compiler sets the bit automatically.
- `Default_Handler` and the weak aliases do **not** run at boot. They run only if the corresponding exception fires.

### 5.2 Memory before my code runs

```
FLASH (non-volatile)                            RAM (volatile, undefined at power-on)
0x00000000  .isr_vector  ← SP and PC come from here     0x20000000  .data  ← needs initial values
            .text        (code)                                     .bss   ← garbage
            .rodata      (string literals)                          ...free...
            .data image  (initial values of .data)      0x20010000  _estack, stack grows DOWN
```

### 5.3 Startup code (`startup.c`)

Responsibilities:

1. Define the **vector table** in section `.isr_vector`.
2. Provide **weak aliases** for every exception handler pointing to `Default_Handler`, so I can override any of them just by defining a function with the same name (this is how `SysTick_Handler` and later `PendSV_Handler` and `SVC_Handler` are hooked in).
3. `Reset_Handler`: copy `.data`, zero `.bss`, call `main()`, then loop forever.

Vector table layout (Cortex-M3 system exceptions):

| Index | Entry | Note |
|---|---|---|
| 0 | Initial stack pointer (`_estack`) | Loaded into SP by hardware |
| 1 | `Reset_Handler` | Loaded into PC by hardware |
| 2 | NMI | |
| 3 | HardFault | |
| 4 | MemManage | |
| 5 | BusFault | |
| 6 | UsageFault | |
| 7-10 | Reserved | |
| 11 | SVCall | Used for system calls (Phase 1) |
| 12 | DebugMon | |
| 13 | Reserved | |
| 14 | PendSV | Context switch (Phase 1) |
| 15 | SysTick | Scheduler tick |
| 16+ | External IRQs | Added when needed |

```c
__attribute__((section(".isr_vector"), used))
const isr_t vector_table[] = {
    (isr_t)&_estack,     /* 0: initial stack pointer */
    Reset_Handler,       /* 1: reset */
    NMI_Handler,         /* 2 */
    HardFault_Handler,   /* 3 */
    /* ... */
};
```

**Why copy `.data`?** `int g_data = 0x1234;` must live in RAM (it is writable), but RAM is empty at power-on. The linker stores the initial values in flash, and the reset handler copies them into RAM.

**Why zero `.bss`?** The C standard guarantees uninitialized globals start at 0. The linker only records where `.bss` is, and the startup code zero-fills it. QEMU usually starts with zeroed RAM, so a missing `.bss` loop would go unnoticed in the emulator and break on real hardware. The loop is written anyway.

**Symbols are addresses, not variables.** `extern uint32_t _sdata;` does not declare a variable. The linker defines the symbol so that its *address* is the value I want, so the code always uses `&_sdata` and never `_sdata`.

### 5.4 Linker script (`linker.ld`)

The linker script tells the linker where things live in memory. Every section has two addresses:

| Address | Meaning |
|---|---|
| **VMA** (run address) | Where the section lives while the program runs |
| **LMA** (load address) | Where its bytes are stored in the image |

For `.text`, VMA equals LMA. For `.data`, they differ: it runs in RAM but its initial values are stored in flash. The reset handler's copy loop moves the data from LMA to VMA.

Symbols exported by the script (any `name = expression;` becomes a global symbol):

| Symbol | Meaning | Lives in |
|---|---|---|
| `_estack` | Top of RAM (`0x20010000`), initial stack pointer | RAM |
| `_sidata` | Start of `.data` initial values (`LOADADDR(.data)`) | FLASH (copy source) |
| `_sdata` / `_edata` | Start / end of `.data` at runtime | RAM |
| `_sbss` / `_ebss` | Start / end of `.bss` | RAM |

Key details:

- Sections are placed **in the order written**, so `.isr_vector` goes first and lands at `0x0`.
- `KEEP(*(.isr_vector))` prevents `--gc-sections` from discarding the table (nothing in C references it, because the hardware reads it).
- `. = ALIGN(4)` matters because the copy and zero loops step in 4-byte words.
- `> RAM AT> FLASH` means "run in RAM, store in flash".
- `.` inside a section is the current VMA, which is how `_sdata`, `_edata`, `_sbss` and `_ebss` are captured.

### 5.5 UART driver (`uart.c`)

UART0 on `lm3s6965evb` is memory-mapped at `0x4000C000`:

| Register | Offset | Use |
|---|---|---|
| `DR` (data) | `0x000` | Write a byte to transmit |
| `FR` (flags) | `0x018` | Bit 5 (`TXFF`): transmit FIFO full |

- Registers are accessed through `volatile` pointers so the compiler never optimizes away hardware reads and writes.
- `uart_putc` waits until the FIFO has room, then writes. `\n` is translated to `\r\n` in one place.
- QEMU needs no clock or baud-rate setup. **Real hardware does**, so this driver is QEMU-specific for now.

### 5.6 Build system (`Makefile`)

Key compile flags:

| Flag | Why |
|---|---|
| `-mcpu=cortex-m3 -mthumb` | Target core, Thumb instruction set |
| `-ffreestanding -nostdlib -nostartfiles` | No hosted C environment, no libc, own startup code |
| `-g -O0` | Debug info, no optimization (sane GDB stepping) |
| `-Wall -Wextra` | Warnings on, and every warning is read |
| `-ffunction-sections -fdata-sections` + `-Wl,--gc-sections` | Drop unused code and data |
| `-MMD -MP` | Generate `.d` dependency files so header edits trigger rebuilds |
| `-Wl,-Map=kernel.map` | Produce a map file for inspecting layout |

Targets: `make` (build), `make run`, `make debug` (QEMU paused, GDB server on `:1234`), `make check` (dump sections and symbols), `make clean`.

```bash
make run          # exit QEMU with Ctrl-A then X
make debug        # terminal 1
gdb-multiarch kernel.elf   # terminal 2, then: target remote :1234
```

### 5.7 Verifying startup and linker correctness

```bash
arm-none-eabi-objdump -h kernel.elf        # section table
arm-none-eabi-nm kernel.elf | grep -E "_estack|_sidata|_sdata|_edata|_sbss|_ebss"
arm-none-eabi-objdump -s -j .isr_vector kernel.elf
```

| Check | Expected |
|---|---|
| `.isr_vector` VMA / LMA | `0x00000000` |
| `.data` VMA vs LMA | VMA in RAM (`0x2000....`), LMA in flash. If equal, `AT> FLASH` is wrong. |
| `_estack` | `0x20010000` |
| `_sdata` | `0x20000000` |
| `_sidata` | In flash, not `0x2000....` |
| Layout order | `_edata` ≥ `_sdata`, `_sbss` ≥ `_edata`, `_ebss` ≥ `_sbss` |
| Vector table word 0 | Bytes `00 00 01 20` (little-endian `0x20010000`) |
| Vector table word 1 | Odd address (Thumb bit set) |

**GDB check, poisoning the variables.** QEMU zeroes RAM, so poison the variables to test *my* code and not QEMU's defaults:

```
(gdb) target remote :1234
(gdb) info registers sp pc            # sp = 0x20010000, pc = Reset_Handler
(gdb) set var g_bss  = 0xdeadbeef
(gdb) set var g_data = 0
(gdb) break main
(gdb) continue
(gdb) p/x g_data                      # expect 0x1234 → .data copy works
(gdb) p/x g_bss                       # expect 0x0    → .bss zeroing works
```

### 5.8 Remaining steps in Phase 0

| Step | Deliverable | Success check |
|---|---|---|
| A | `linker.ld`, `startup.c`, `main.c` with `uart_puts("Hello")` | "Hello" appears in the terminal (**done**) |
| B | `kprintf` supporting `%s %c %d %u %x` (exercise: `%p` and zero-padded `%08x`) | Prints `g_data` and `g_bss` values, proving startup works |
| C | SysTick + `SysTick_Handler` + `wfi` loop | `tick=N` printed periodically |
| D | GDB session | Break in `SysTick_Handler`, inspect `tick_count`, view the vector table with `x/16xw 0x0` |
| E (stretch) | `HardFault_Handler` printing the stacked PC, LR and xPSR | A deliberate bad write reports where it happened |

#### `kprintf`

- Uses `<stdarg.h>`, which is compiler-provided and works with `-ffreestanding`.
- Integer-to-text conversion: `v % base` gives the last digit, `v / base` drops it, digits are buffered and printed in reverse.
- Negation as `0u - (uint32_t)v` avoids signed-overflow undefined behavior for `INT_MIN`.
- Cortex-M3 has hardware `UDIV`. A Cortex-M0 would need `libgcc` for division.

#### SysTick

A 24-bit down-counter in the core. When it hits 0 it reloads and raises exception 15.

| Register | Address | Purpose |
|---|---|---|
| `CSR` | `0xE000E010` | bit 0 ENABLE, bit 1 TICKINT, bit 2 CLKSOURCE |
| `RVR` | `0xE000E014` | Reload value (max `0xFFFFFF`) |
| `CVR` | `0xE000E018` | Current value (any write clears it) |

- Initialization: stop the timer, set reload to `cycles - 1`, clear the current value, then enable counter and interrupt.
- `tick_count` is `volatile` because the ISR and `main` share it.
- Defining a **strong** `SysTick_Handler` overrides the weak alias in `startup.c` at link time, so `startup.c` is not touched.
- Interrupts are enabled at reset, and ISRs are plain C functions because the hardware stacks and unstacks caller-saved registers.
- QEMU is not cycle-accurate, so tune `CYCLES_PER_TICK` only for a visible rate.

#### Final `main` for Phase 0

1. Print a banner and the startup self-test (`g_data` = `0x1234`, `g_bss` = `0`).
2. `systick_init(CYCLES_PER_TICK)`.
3. Loop: `wfi`, and every N ticks print `tick=<n>`.

### 5.9 Phase 0 "done when"

Periodic `tick=N` output driven by SysTick, the startup self-test passing, and a GDB session that breaks in `SysTick_Handler` and inspects state.

### 5.10 Common pitfalls

| Symptom | Likely cause |
|---|---|
| Vector table not at `0x0` | Section name typo or missing `KEEP` |
| Immediate crash on boot | Wrong `_estack` (must be `0x20010000`) |
| Globals contain garbage | `.data` copy or `.bss` zeroing wrong or missing |
| Nothing prints | UART address wrong, or `-nographic` missing |
| Counter never changes in `main` | Missing `volatile` |
| `undefined reference to main` | `main.c` missing from the link |
| `undefined reference to memset` at `-O2` | GCC turned the zero loop into `memset`. Use `-fno-tree-loop-distribute-patterns` or write `memset`. |
| Header edits not picked up | Missing `-MMD -MP` / `-include $(OBJS:.o=.d)` |

### 5.11 Questions to answer

1. Why can `Reset_Handler` use local variables but not an initialized global?
   *(Hint: what has and hasn't been set up at that point?)*
2. What breaks if `vector_table` sits at `0x100` instead of `0x0` with no other changes?
   *(Hint: what does the hardware read at reset?)*

---

## 6. Phase 1: Kernel Core

**Purpose:** turn the bare-metal program into a multitasking kernel.

### What will be built

- **Task Control Block (TCB):** saved stack pointer, state (ready, running, blocked, sleeping), priority, delay counter, and list links.
- **Per-task stacks** and a **task creation API**. Each new task's stack is pre-filled with a fake exception frame so the first context switch "returns" into the task.
- **Context switch in PendSV.** PendSV is the lowest-priority exception, so switching happens after all other interrupts finish. The handler is a small piece of assembly (inline or naked) that saves R4-R11 to the current task's stack, stores PSP in the TCB, picks the next task, and restores.
- **SVC** for system calls, and the first task launched via SVC.
- **Two stack pointers:** MSP for exceptions and kernel, PSP for tasks.
- **Scheduling:** round-robin first, then **fixed-priority preemptive**.
- **Idle task** (runs `wfi`), `delay(ticks)`, and a **tick-based sleep list** driven by SysTick.

### Hard parts

- The context switch assembly and exception-return values (`EXC_RETURN`).
- Debugging **stack corruption** and HardFaults, and that is why Phase 0 step E (HardFault handler) matters.

### Done when

3+ tasks run with preemption and correct priority behavior, verified in GDB.

---

## 7. Phase 2: Synchronization Primitives

**Purpose:** allow tasks to share data and coordinate safely.

### What will be built

- **Semaphores** (binary and counting).
- **Mutexes with priority inheritance.**
- **Message queues.**
- **Software timers.**
- **Priority-inversion demo** (Mars Pathfinder-style scenario) that fails without inheritance and works with it.

### Design notes

- Critical sections through `PRIMASK` or `BASEPRI`, kept short.
- Blocked tasks live on wait lists ordered by priority.

### Done when

Each primitive has a test, and the priority-inversion demo is documented.

---

## 8. Phase 3: Real-Time Scheduling

**Purpose:** make the kernel a real-time scheduler and prove it correct against theory.

### What will be built

- A **pluggable scheduler** through an ops struct of function pointers (a good C design to show off):

  ```c
  typedef struct {
      void  (*add)(tcb_t *t);
      void  (*remove)(tcb_t *t);
      tcb_t*(*pick_next)(void);
      void  (*on_tick)(void);
  } sched_ops_t;
  ```
- A **periodic task API** (period, WCET, relative deadline).
- **RMS** (rate-monotonic, fixed priority) and **EDF** (earliest deadline first, dynamic priority).
- **Deadline-miss detection** and a lightweight **trace buffer** exported over UART.
- A **Python schedulability tool**: utilization bounds (Liu & Layland) and response-time analysis, cross-checked against the kernel's observed behavior.

### Done when

The kernel's measured results agree with the analytical predictions.

---

## 9. Phase 4: Thermal and Power Model

**Purpose:** give the kernel a simulated thermal environment so policies can be evaluated.

### What will be built

- **First-order RC thermal model** in fixed-point C:

  ```
  T[k+1] = T[k] + (Δt / C) · ( P − (T − T_amb) / R )
  ```

  where `C` is thermal capacitance, `R` is thermal resistance, `P` is power and `T_amb` is ambient temperature.
- **DVFS levels** (e.g. 4 frequency/voltage points) with power `P = P_dyn(f, V) + P_leak(T)`, where `P_dyn ∝ C_eff · V² · f`. Task execution time scales with `1/f` in virtual time.
- A **virtual temperature sensor** behind a HAL interface, so policy code reads it like real hardware.
- A **reference implementation in Python** of the same model for validation.
- **POS-flavored synthetic workload:**
  - crypto bursts,
  - display refresh,
  - card-reader polling,
  - **thermal printer head** as a large periodic heat source.

### Done when

The kernel's temperature curve matches the Python reference within a small error.

---

## 10. Phase 5: Thermal-Aware Policies and Experiments

**Purpose:** this is where the research value and resume value come from.

### Policies to implement and compare

| # | Policy | Idea |
|---|---|---|
| 1 | **Baseline** | Max frequency, no management |
| 2 | **Reactive throttling** | Threshold with hysteresis |
| 3 | **PID-based DVFS** | Feedback controller on temperature |
| 4 | **Predictive** | Use the RC model to pick the highest frequency that keeps T below the limit over a horizon |
| 5 | **EDF + slack-based DVFS** | Cycle-conserving style: reclaim slack to lower frequency and save energy without missing deadlines |

### Experiment design

- Several workloads and utilization levels.
- Metrics: **peak temperature**, **time above threshold**, **total energy**, **deadline misses**.
- Plots generated with Python/matplotlib and embedded in the README.

### Done when

There are reproducible plots and a results table showing trade-offs between policies, with numbers like "reduced peak temperature by X% with zero deadline misses".

---

## 11. Phase 6: Polish, Testing, CI, Documentation

### What will be built

- **HAL and drivers** (UART, timer, virtual I2C sensor) with **unit tests on the host** (Unity or CMocka).
- **GitHub Actions CI** that builds and runs QEMU tests headlessly.
- **Static analysis:** `cppcheck` and a MISRA-C-style pass.
- **README** with architecture diagram, methodology, results plots and limitations. Optionally a blog post or short technical report (the thermal work could become a paper).

### Architecture (target)

```
┌───────────────────────────────────────────────┐
│ Apps: POS-style workloads, experiments        │
├───────────────────────────────────────────────┤
│ Thermal: RC model · DVFS · policies           │
├───────────────────────────────────────────────┤
│ Kernel: tasks · schedulers (RMS/EDF) ·        │
│         semaphores · mutexes · queues · timers│
├───────────────────────────────────────────────┤
│ HAL: UART · timer · temperature sensor        │
├───────────────────────────────────────────────┤
│ Arch: startup · vector table · PendSV · SVC   │
├───────────────────────────────────────────────┤
│ QEMU: Cortex-M3 (lm3s6965evb)                 │
└───────────────────────────────────────────────┘
```

---

## 12. Methodology and Limitations

- **QEMU is not cycle-accurate.** Timing uses **virtual time**: SysTick ticks plus a modeled cycle cost per task. This is a valid methodology and is common in scheduling research, and it will be stated clearly in the final README.
- **The thermal model is simulated**, not measured. Optional improvement: buy a cheap board (Pico, ESP32 or STM32 Nucleo, about $10-20) with a temperature sensor and validate the RC model against real measurements.
- **The UART driver is QEMU-specific** (no clock or baud setup). Real hardware needs both.
- **gem5 is not used** in this project. It targets application-class cores and is better suited to a separate microarchitecture study.
- **Scope creep risk:** finish the MVP (phases 0 to 3) before starting thermal policies.

---

## 13. Timeline

| Milestone | Estimate |
|---|---|
| MVP (phases 0-3 + basic thermal model) | About 6-7 weeks part-time |
| Full project | About 130-160 hours |
| At 10-12 h/week | 12-14 weeks |
| At 15 h/week | 9-10 weeks |
| Full-time | About 4 weeks |

Extra time should be budgeted for the context-switch assembly and stack-corruption debugging.

---

## 14. Progress Checklist

### Environment
- [x] Download and configure QEMU (ARM target only)
- [x] Build QEMU
- [x] Install `arm-none-eabi` toolchain
- [ ] `qemu-system-arm` reachable from `make run` (install or set `QEMU=`)

### Phase 0
- [x] `startup.c` (vector table, weak handlers, `Reset_Handler`)
- [x] `linker.ld` (memory map, `.data`/`.bss` symbols)
- [x] Verified layout with `objdump` / `nm` / GDB
- [x] `uart.c` and Hello output code
- [x] Makefile with `run`, `debug`, `check`
- [ ] "Hello" confirmed on the terminal
- [ ] `kprintf` (`%s %c %d %u %x`)
- [ ] Startup self-test printed (`g_data`, `g_bss`)
- [ ] SysTick + `SysTick_Handler`
- [ ] Periodic `tick=N` output
- [ ] GDB session (break in ISR, inspect `tick_count`, dump vector table)
- [ ] HardFault handler (stretch)

### Later phases
- [ ] Phase 1: Kernel core
- [ ] Phase 2: Synchronization primitives
- [ ] Phase 3: Real-time schedulers and analysis
- [ ] Phase 4: Thermal and power model
- [ ] Phase 5: Thermal-aware policies and experiments
- [ ] Phase 6: Tests, CI, documentation

---

## 15. Resume Bullets

Once the project is complete:

- Designed and implemented a preemptive RTOS kernel for Cortex-M3 in C with pluggable RMS/EDF schedulers and priority-inheritance mutexes.
- Built a thermal/power simulation and evaluated five thermal-management policies, reducing peak temperature by **X%** with zero deadline misses.
- Wrote startup code, linker scripts, context-switch assembly and drivers without vendor HAL.
- Automated QEMU-based regression testing in CI with static analysis (cppcheck, MISRA-style checks).