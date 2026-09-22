CC = arm-none-eabi-gcc
QEMU ?= qemu-system-arm

BUILD_DIR = build

SRCS = startup.c main.c uart.c kprintf.c systick.c fault_handler.c
OBJS = $(addprefix $(BUILD_DIR)/,$(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)

CFLAGS = -mcpu=cortex-m3 -mthumb -ffreestanding -g -O0 -Wall -Wextra \
 -ffunction-sections -fdata-sections -MMD -MP
LDFLAGS = -T linker.ld -nostdlib -nostartfiles \
 -Wl,-Map=$(BUILD_DIR)/kernel.map -Wl,--gc-sections

ELF = $(BUILD_DIR)/kernel.elf

all: $(ELF)

$(ELF): $(OBJS) linker.ld | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(ELF)
	$(QEMU) -M lm3s6965evb -kernel $(ELF) -nographic

debug: $(ELF)
	$(QEMU) -M lm3s6965evb -kernel $(ELF) -nographic -S -gdb tcp::1234

check: $(ELF)
	arm-none-eabi-objdump -h $(ELF)
	arm-none-eabi-nm $(ELF) | grep -E "_estack|_sidata|_sdata|_edata|_sbss|_ebss"

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run debug check clean

-include $(DEPS)