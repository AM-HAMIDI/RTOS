CC      = arm-none-eabi-gcc
QEMU   ?= qemu-system-arm

CFLAGS  = -mcpu=cortex-m3 -mthumb -ffreestanding -g -O0 -Wall -Wextra \
          -ffunction-sections -fdata-sections
LDFLAGS = -T linker.ld -nostdlib -nostartfiles \
          -Wl,-Map=kernel.map -Wl,--gc-sections

kernel.elf: startup.o main.o linker.ld
	$(CC) $(CFLAGS) $(LDFLAGS) startup.o main.o -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

check: kernel.elf
	arm-none-eabi-objdump -h kernel.elf
	arm-none-eabi-nm kernel.elf | grep -E "_estack|_sidata|_sdata|_edata|_sbss|_ebss"
	arm-none-eabi-objdump -s -j .isr_vector kernel.elf

debug: kernel.elf
	$(QEMU) -M lm3s6965evb -kernel kernel.elf -nographic -S -gdb tcp::1234

clean:
	rm -f *.o kernel.elf kernel.map