CC      = arm-none-eabi-gcc
QEMU   ?= qemu-system-arm

BUILD_DIR = build

SRCS = $(wildcard arch/*.c) $(wildcard hal/*.c) $(wildcard kernel/*.c) main.c
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

INCLUDES = -Iarch -Ihal -Ikernel -I.

CFLAGS  = -mcpu=cortex-m3 -mthumb -ffreestanding -g -O0 -Wall -Wextra \
          -ffunction-sections -fdata-sections $(INCLUDES) \
          -MMD -MP

LDFLAGS = -mcpu=cortex-m3 -mthumb -nostdlib -Wl,--gc-sections

.PHONY: all clean run

all: $(BUILD_DIR)/main.elf

$(BUILD_DIR)/main.elf: $(OBJS)
	$(CC) $(LDFLAGS) -T arch/linker.ld -o $@ $^

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

run: $(BUILD_DIR)/main.elf
	$(QEMU) -M lm3s6965evb -nographic -kernel $<

clean:
	rm -rf $(BUILD_DIR)