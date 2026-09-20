CC      = arm-none-eabi-gcc
QEMU   ?= qemu-system-arm

SRCS    = startup.c main.c uart.c
OBJS    = $(SRCS:.c=.o)

CFLAGS  = -mcpu=cortex-m3 -mthumb -ffreestanding -g -O0 -Wall -Wextra \
          -ffunction-sections -fdata-sections -MMD -MP
LDFLAGS = -T linker.ld -nostdlib -nostartfiles \
          -Wl,-Map=kernel.map -Wl,--gc-sections

kernel.elf: $(OBJS) linker.ld
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: kernel.elf
	$(QEMU) -M lm3s6965evb -kernel kernel.elf -nographic

debug: kernel.elf
	$(QEMU) -M lm3s6965evb -kernel kernel.elf -nographic -S -gdb tcp::1234

check: kernel.elf
	arm-none-eabi-objdump -h kernel.elf
	arm-none-eabi-nm kernel.elf | grep -E "_estack|_sidata|_sdata|_edata|_sbss|_ebss"

clean:
	rm -f *.o *.d kernel.elf kernel.map

-include $(OBJS:.o=.d)