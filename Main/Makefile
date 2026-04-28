CC = i686-elf-gcc
AS = nasm
OBJCOPY = i686-elf-objcopy
QEMU = qemu-system-i386

CFLAGS = -ffreestanding -m32 -Wall -Wextra -Iinclude
LDFLAGS = -Tboot/linker.ld -ffreestanding -m32 -nostdlib

OBJS = \
	build/entry.o \
	build/interrupts.o \
	build/main.o \
	build/console.o \
	build/idt.o \
	build/irq.o \
	build/timer.o \
	build/keyboard.o \
	build/shell.o \
	build/panic.o \
	build/paging.o \
	build/memory.o \
	build/scheduler.o\
	build/test.o\
	build/fs.o


all: build/os-image.bin

build:
	mkdir -p build

build/boot.bin: boot/boot.asm | build
	$(AS) -f bin boot/boot.asm -o build/boot.bin

build/entry.o: kernel/entry.asm | build
	$(AS) -f elf32 kernel/entry.asm -o build/entry.o

build/interrupts.o: kernel/interrupts.asm | build
	$(AS) -f elf32 kernel/interrupts.asm -o build/interrupts.o

build/%.o: kernel/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build/kernel.elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o build/kernel.elf

build/kernel.bin: build/kernel.elf
	$(OBJCOPY) -O binary build/kernel.elf build/kernel.bin

build/os-image.bin: build/boot.bin build/kernel.bin
	dd if=/dev/zero of=build/os-image.bin bs=512 count=2880
	dd if=build/boot.bin of=build/os-image.bin conv=notrunc
	dd if=build/kernel.bin of=build/os-image.bin bs=512 seek=1 conv=notrunc

run: all
	$(QEMU) -drive format=raw,file=build/os-image.bin

size: build/kernel.bin
	stat -c%s build/kernel.bin

clean:
	rm -rf build

rebuild: clean all

.PHONY: all run size clean rebuild