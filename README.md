/kernel     (sched, task, sync, timer)
/arch       (cortex-m port: startup, PendSV, linker)
/hal        (uart, timer, temp sensor)
/thermal    (RC model, DVFS, policies)
/apps       (workloads, experiments)
/tools      (python: analysis, plots)
/tests      (host + QEMU tests)


Pick: Thermal-Aware RTOS Kernel on Cortex-M (in QEMU)

This is option 1, built on a small kernel you write yourself (option 2), with the drivers and tests (option 6) as part of it. It's one coherent project, and it plays to your strengths: C, low-level debugging, and your thermal research. I'd leave option 7 (embedded Linux) for later as a separate weekend-sized project.

Goal: a preemptive RTOS kernel for Cortex-M3 that runs RMS/EDF scheduling, simulates a thermal and power model, and compares thermal-management policies on measurable metrics (peak temperature, energy, deadline misses).

Setup
Target: Cortex-M3 on QEMU (lm3s6965evb or mps2-an385)
Toolchain: arm-none-eabi-gcc, GDB, Make or CMake, Python and matplotlib for analysis
Timing caveat: QEMU isn't cycle-accurate, so use virtual time (SysTick ticks plus a modeled cycle cost per task) and state that in your README. It's a valid methodology, and it's what most scheduling papers do.
Roadmap
Phase	Weeks*	Focus	Est. hours
0	1	Bare-metal foundation	10-12
1	2-3	Kernel core	20-25
2	4	Synchronization primitives	15
3	5-6	Real-time schedulers and analysis	20
4	7-8	Thermal and power model	20
5	9-10	Thermal-aware policies and experiments	25
6	11-12	Drivers, tests, CI, documentation	20

*Assuming about 10-12 hours per week.

Phase 0: Bare-metal foundation
Startup code, vector table, and linker script written by hand (no vendor HAL)
UART output (or semihosting) for printf-style logging
QEMU plus GDB debugging workflow (-S -gdb tcp::1234)
Done when: the blinking-equivalent works, meaning periodic UART output driven by SysTick
Phase 1: Kernel core
Task control block, per-task stacks, and task creation API
Context switch in PendSV (a small piece of inline assembly), plus SVC for system calls
Round-robin first, then fixed-priority preemptive scheduling
Idle task, delay(ticks), and a tick-based sleep list
Done when: 3+ tasks run with preemption and correct priority behavior, verified in GDB
Phase 2: Synchronization primitives
Semaphores, mutexes with priority inheritance, and message queues
Software timers
A priority-inversion demo (a Mars Pathfinder-style scenario) that fails without inheritance and works with it
Done when: each primitive has a test and the inversion demo is documented
Phase 3: Real-time scheduling
Make the scheduler pluggable via an ops struct of function pointers, which is a good C design to show off
Implement a periodic task API (period, WCET, deadline), RMS, and EDF
Deadline-miss detection and a lightweight trace buffer, exported over UART
A Python tool for schedulability analysis (utilization bounds and response-time analysis) that cross-checks what the kernel does
Done when: the kernel's results agree with the analytical predictions
Phase 4: Thermal and power model
First-order RC thermal model in fixed-point C: T[k+1] = T[k] + (Δt/C)·(P − (T − T_amb)/R)
DVFS levels (e.g., 4 frequency/voltage points) with power P = P_dyn(f, V) + P_leak(T). Task execution time scales with 1/f in virtual time.
A virtual temperature sensor behind a HAL interface, so policy code reads it like real hardware
Validate against a reference implementation of the same model in Python
Make the synthetic workload POS-flavored: crypto bursts, display refresh, card-reader polling, and a thermal printer head as a large periodic heat source. This ties the project to your job.
Done when: the temperature curve matches the Python reference within a small error
Phase 5: Thermal-aware policies and experiments

Implement and compare:

Baseline: max frequency, no management
Reactive throttling: threshold with hysteresis
PID-based DVFS
Predictive: use the RC model to pick the highest frequency that keeps T below the limit over a horizon
EDF with slack-based DVFS (in the spirit of cycle-conserving DVFS), which saves energy without missing deadlines

Run each across several workloads and utilization levels. Report peak temperature, time above threshold, total energy, and deadline misses. Plot everything. This phase is where the resume value comes from.

Phase 6: Polish
HAL and drivers (UART, timer, virtual I2C sensor) with unit tests on the host (Unity or CMocka)
GitHub Actions CI that builds and runs QEMU tests headlessly
cppcheck and a MISRA-C-style pass
README with an architecture diagram, methodology, results plots, and limitations; optionally a blog post
Timeline
MVP (phases 0-3 plus a basic thermal model): about 6-7 weeks part-time. This is already a solid resume project.
Full project: about 130-160 hours, so roughly 12-14 weeks at 10-12 h/week, 9-10 weeks at 15 h/week, or about 4 weeks full-time.
As an intermediate-to-advanced C developer you'll move faster through the tooling and data structures. The hard parts are the context-switch assembly and debugging stack corruption, so budget extra time there.
