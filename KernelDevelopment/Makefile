# Output Files
BOOT = boot.bin
KERNEL_BIN = kernel.bin
IMAGE = BlebaImage.bin

# Input Files
BOOT_SRC = BlebaBoot.asm
KERNEL_ENTRY_SRC = kernel.asm
KERNEL_C_SRC = Kernel.c

# Build Tools (Using system defaults with 32-bit flags)
ASM = nasm
CC = gcc
LD = ld
# -m32: 32-bit mode | -ffreestanding: No standard library | -fno-pie: Disable position independent code
CFLAGS = -m32 -ffreestanding -fno-pie -c
# -m elf_i386: 32-bit linker | -Ttext: Where code starts | --oformat binary: Flat binary output
LDFLAGS = -m elf_i386 -s -Ttext 0x10000 --oformat binary

all: $(IMAGE)

# 1. Assemble the Bootloader
$(BOOT): $(BOOT_SRC)
	$(ASM) -f bin $(BOOT_SRC) -o $(BOOT)

# 2. Assemble Kernel Entry Stub (ELF object)
k_entry.o: $(KERNEL_ENTRY_SRC)
	$(ASM) -f elf32 $(KERNEL_ENTRY_SRC) -o k_entry.o

# 3. Compile C Kernel (ELF object)
k_main.o: $(KERNEL_C_SRC)
	$(CC) $(CFLAGS) $(KERNEL_C_SRC) -o k_main.o

# 4. Link everything into a flat binary kernel
$(KERNEL_BIN): k_entry.o k_main.o
	$(LD) $(LDFLAGS) -o $(KERNEL_BIN) k_entry.o k_main.o

# 5. Create the final disk image
$(IMAGE): $(BOOT) $(KERNEL_BIN)
	cat $(BOOT) $(KERNEL_BIN) > $(IMAGE)

# Clean up
clean:
	rm -rf *.bin *.o $(IMAGE)

# Run in QEMU
run: $(IMAGE)
	qemu-system-x86_64 -drive format=raw,file=$(IMAGE)