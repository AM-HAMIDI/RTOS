# ============ Main configuration ============
cmake_minimum_required(VERSION 3.20)
project(RTOS C ASM)

set(TARGET main.elf)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# ============ Build executable target ============
file(GLOB_RECURSE ARCH_SRCS   CONFIGURE_DEPENDS "arch/*.c")
file(GLOB_RECURSE HAL_SRCS    CONFIGURE_DEPENDS "hal/*.c")
file(GLOB_RECURSE KERNEL_SRCS CONFIGURE_DEPENDS "kernel/*.c")

set(SRCS
    ${ARCH_SRCS}
    ${HAL_SRCS}
    ${KERNEL_SRCS}
    main.c
)

add_executable(${TARGET} ${SRCS})

# ============ Compile options ============
target_compile_options(${TARGET} PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -ffreestanding
    -g
    -O0
    -Wall
    -Wextra
    -ffunction-sections
    -fdata-sections
)


target_include_directories(${TARGET} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    arch
    hal
    kernel
)

# ============ Link options ============
set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/arch/linker.ld")
set(MAP_FILE      "${CMAKE_BINARY_DIR}/${TARGET}.map")

target_link_options(${TARGET} PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -nostdlib
    -nostartfiles
    -Wl,--gc-sections
    -Wl,-Map=${MAP_FILE}
    -T${LINKER_SCRIPT}
)

set_target_properties(${TARGET} PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

# ---- Post-build: print section sizes --------------------------------------
if(CMAKE_SIZE)
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${TARGET}>
        COMMENT "Section sizes"
    )
endif()

# ---- Convenience targets ---------------------------------------------------

# Equivalent to `make run`
find_program(QEMU_BIN qemu-system-arm)
if(QEMU_BIN)
    add_custom_target(run
        COMMAND ${QEMU_BIN} -M lm3s6965evb -nographic -kernel $<TARGET_FILE:${TARGET}>
        DEPENDS ${TARGET}
        USES_TERMINAL           # gives QEMU a real terminal (Ctrl-A X works)
        COMMENT "Starting QEMU emulation..."
    )

    # Equivalent to `make debug`: QEMU pauses, waits for GDB on :1234
    add_custom_target(debug
        COMMAND ${QEMU_BIN} -M lm3s6965evb -nographic -kernel $<TARGET_FILE:${TARGET}> -S -gdb tcp::1234
        DEPENDS ${TARGET}
        USES_TERMINAL
        COMMENT "QEMU paused, waiting for GDB on :1234 ..."
    )
endif()

# Equivalent to `make check`: dump sections and key linker symbols
if(CMAKE_OBJDUMP AND CMAKE_NM)
    add_custom_target(check
        COMMAND ${CMAKE_OBJDUMP} -h $<TARGET_FILE:${TARGET}>
        COMMAND ${CMAKE_NM} $<TARGET_FILE:${TARGET}> | grep -E "_estack|_sidata|_sdata|_edata|_sbss|_ebss" || true
        DEPENDS ${TARGET}
        VERBATIM
        COMMENT "Checking sections and linker symbols..."
    )
endif()