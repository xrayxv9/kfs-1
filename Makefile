# ==========================================
# Configuration Variables
# ==========================================
OS_NAME = kfs1

# Toolchain
AS = i386-elf-as
CC = i386-elf-gcc
LD = i386-elf-gcc

# Compiler & Assembler Flags
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -T LINKER/linker.ld -ffreestanding -O2 -nostdlib

# Source and Object Files
ASM_SRC = ASM/boot.s ASM/interrupt.s
C_SRC = C/kernel.c
HEADERS = C/multiboot.h
ASM_OBJ = boot.o
C_OBJ = kernel.o

# Output Binaries
KERNEL_BIN = $(OS_NAME)
ISO_IMAGE = github_$(OS_NAME)_iso/$(OS_NAME).iso
ISODIR = isodir

# ==========================================
# Build Rules
# ==========================================

.PHONY: all clean fclean

all: $(ISO_IMAGE)

# 1. Compile Assembly bootloader
$(ASM_OBJ): $(ASM_SRC)
        $(AS) $< -o $@

# 2. Compile C kernel
$(C_OBJ): $(C_SRC) $(HEADERS)
        $(CC) -c $< -o $@ $(CFLAGS)

# 3. Link kernel binary
$(KERNEL_BIN): $(ASM_OBJ) $(C_OBJ)
        $(LD) $(LDFLAGS) -o $@ $^ -lgcc

# 4. Create ISO image
$(ISO_IMAGE): $(KERNEL_BIN)
        mkdir -p $(ISODIR)/boot/grub
        cp $(KERNEL_BIN) $(ISODIR)/boot/$(OS_NAME)
        grub-mkrescue -o $@ $(ISODIR)

# Clean up object files and kernel binary (keeps isodir and grub.cfg safe)
clean:
        rm -f $(ASM_OBJ) $(C_OBJ) $(KERNEL_BIN)

# Thorough clean (removes object files, binaries, and the ISO image, but leaves isodir/grub.cfg intact)
fclean: clean
        rm -f $(ISO_IMAGE)
