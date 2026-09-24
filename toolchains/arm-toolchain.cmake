# ============ Cross-compiling to bare-metal ARM ============
set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# ============ Extra tools (direct assignment) ============
set(CMAKE_OBJCOPY arm-none-eabi-objcopy CACHE INTERNAL "")
set(CMAKE_OBJDUMP arm-none-eabi-objdump CACHE INTERNAL "")
set(CMAKE_SIZE    arm-none-eabi-size    CACHE INTERNAL "")
set(CMAKE_NM      arm-none-eabi-nm      CACHE INTERNAL "")
set(CMAKE_GDB     gdb-multiarch         CACHE INTERNAL "")

# ============ CMake configurations ============
set(CMAKE_C_FLAGS_INIT   "-mcpu=cortex-m3 -mthumb")
set(CMAKE_ASM_FLAGS_INIT "-mcpu=cortex-m3 -mthumb")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)