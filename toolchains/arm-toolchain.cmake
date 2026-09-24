# ============ Cross-compiling to bare-metal ARM ============
set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# ============ Extra tools ============
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy)
find_program(CMAKE_OBJDUMP arm-none-eabi-objdump)
find_program(CMAKE_SIZE    arm-none-eabi-size)
find_program(CMAKE_NM      arm-none-eabi-nm)
find_program(CMAKE_GDB     gdb-multiarch)


# ============ CMake configurations ============
set(CMAKE_C_FLAGS_INIT   "-mcpu=cortex-m3 -mthumb")
set(CMAKE_ASM_FLAGS_INIT "-mcpu=cortex-m3 -mthumb")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)