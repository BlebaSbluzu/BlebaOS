# --------------------------------------------------
# BlebaOS Makefile
# Stage 4 build: bootloader + kernel + shell
# --------------------------------------------------

# Tools
CC       = i686-elf-gcc
AS       = nasm
OBJCOPY  = i686-elf-objcopy
QEMU     = qemu-system-i386

# Directories
BOOT_DIR    = boot
KERNEL_DIR  = kernel
INCLUDE_DIR = include
BUILD_DIR   = build

# Output files
BOOT_BIN    = $(BUILD_DIR)/boot.bin
KERNEL_ELF  = $(BUILD_DIR)/kernel.elf
KERNEL_BIN  = $(BUILD_DIR)/kernel.bin
OS_IMAGE    = $(BUILD_DIR)/os-image.bin

# Compiler / linker flags
CFLAGS   = -ffreestanding -m32 -Wall -Wextra -I$(INCLUDE_DIR)
LDFLAGS  = -T$(BOOT_DIR)/linker.ld -ffreestanding -m32 -nostdlib

# Source files
KERNEL_ASM_SRC = \
	$(KERNEL_DIR)/entry.asm \
	$(KERNEL_DIR)/interrupts.asm

KERNEL_C_SRC = \
	$(KERNEL_DIR)/main.c \
	$(KERNEL_DIR)/console.c \
	$(KERNEL_DIR)/idt.c \
	$(KERNEL_DIR)/irq.c \
	$(KERNEL_DIR)/timer.c \
	$(KERNEL_DIR)/keyboard.c \
	$(KERNEL_DIR)/shell.c \
	$(KERNEL_DIR)/panic.c

# Object files
KERNEL_ASM_OBJ = \
	$(BUILD_DIR)/entry.o \
	$(BUILD_DIR)/interrupts.o

KERNEL_C_OBJ = \
	$(BUILD_DIR)/main.o \
	$(BUILD_DIR)/console.o \
	$(BUILD_DIR)/idt.o \
	$(BUILD_DIR)/irq.o \
	$(BUILD_DIR)/timer.o \
	$(BUILD_DIR)/keyboard.o \
	$(BUILD_DIR)/shell.o \
	$(BUILD_DIR)/panic.o

KERNEL_OBJ = $(KERNEL_ASM_OBJ) $(KERNEL_C_OBJ)

# Default target
all: $(OS_IMAGE)

# Make sure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# --------------------------------------------------
# Bootloader
# --------------------------------------------------
$(BOOT_BIN): $(BOOT_DIR)/boot.asm | $(BUILD_DIR)
	$(AS) -f bin $< -o $@

# --------------------------------------------------
# Kernel assembly
# --------------------------------------------------
$(BUILD_DIR)/entry.o: $(KERNEL_DIR)/entry.asm | $(BUILD_DIR)
	$(AS) -f elf32 $< -o $@

$(BUILD_DIR)/interrupts.o: $(KERNEL_DIR)/interrupts.asm | $(BUILD_DIR)
	$(AS) -f elf32 $< -o $@

# --------------------------------------------------
# Kernel C files
# --------------------------------------------------
$(BUILD_DIR)/main.o: $(KERNEL_DIR)/main.c $(INCLUDE_DIR)/console.h $(INCLUDE_DIR)/idt.h $(INCLUDE_DIR)/irq.h $(INCLUDE_DIR)/timer.h $(INCLUDE_DIR)/keyboard.h $(INCLUDE_DIR)/shell.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/console.o: $(KERNEL_DIR)/console.c $(INCLUDE_DIR)/common.h $(INCLUDE_DIR)/console.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/idt.o: $(KERNEL_DIR)/idt.c $(INCLUDE_DIR)/idt.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/irq.o: $(KERNEL_DIR)/irq.c $(INCLUDE_DIR)/common.h $(INCLUDE_DIR)/console.h $(INCLUDE_DIR)/irq.h $(INCLUDE_DIR)/panic.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/timer.o: $(KERNEL_DIR)/timer.c $(INCLUDE_DIR)/common.h $(INCLUDE_DIR)/console.h $(INCLUDE_DIR)/irq.h $(INCLUDE_DIR)/timer.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/keyboard.o: $(KERNEL_DIR)/keyboard.c $(INCLUDE_DIR)/common.h $(INCLUDE_DIR)/console.h $(INCLUDE_DIR)/irq.h $(INCLUDE_DIR)/shell.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/shell.o: $(KERNEL_DIR)/shell.c $(INCLUDE_DIR)/console.h $(INCLUDE_DIR)/shell.h $(INCLUDE_DIR)/timer.h $(INCLUDE_DIR)/panic.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/panic.o: $(KERNEL_DIR)/panic.c $(INCLUDE_DIR)/console.h $(INCLUDE_DIR)/panic.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# --------------------------------------------------
# Link kernel
# --------------------------------------------------
$(KERNEL_ELF): $(KERNEL_OBJ)
	$(CC) $(LDFLAGS) $(KERNEL_OBJ) -o $@

# Convert kernel ELF -> raw binary
$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

# --------------------------------------------------
# Build floppy image
# --------------------------------------------------
$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	dd if=/dev/zero of=$(OS_IMAGE) bs=512 count=2880
	dd if=$(BOOT_BIN) of=$(OS_IMAGE) conv=notrunc
	dd if=$(KERNEL_BIN) of=$(OS_IMAGE) bs=512 seek=1 conv=notrunc

# --------------------------------------------------
# Convenience targets
# --------------------------------------------------
run: $(OS_IMAGE)
	$(QEMU) -drive format=raw,file=$(OS_IMAGE)

debug: $(OS_IMAGE)
	$(QEMU) -drive format=raw,file=$(OS_IMAGE) -monitor stdio

size: $(KERNEL_BIN)
	stat -c%s $(KERNEL_BIN)

clean:
	rm -rf $(BUILD_DIR)/*

rebuild: clean all

.PHONY: all run debug size clean rebuild