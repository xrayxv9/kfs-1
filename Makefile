# ==========================================
# Configuration Variables
# ==========================================
OS_NAME = kfs1


# Toolchain
NASM = nasm
AS = i386-elf-as
CC = i386-elf-gcc
LD = i386-elf-gcc

# Compiler & Assembler Flags
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -T LINKER/linker.ld -ffreestanding -O2 -nostdlib
NASM_FLAGS = -f elf32

# Paths
OBJ_PATH = obj/
BOOT_PATH = BOOT_ASM/
ASM_PATH = ASM/
C_PATH = C/
FONT_PATH = FONTS/

# Source Files (noms de fichiers seuls)
ASM_BOOT = boot.s
ASM_INT = interrupt.s
C_KERNEL = kernel.c
FONT_FILE = Lat15-Terminus16.psf
HEADERS = $(C_PATH)multiboot.h

# Full Source Paths
ASM_BOOT_SRC = $(addprefix $(BOOT_PATH), $(ASM_BOOT))
ASM_SRC = $(addprefix $(ASM_PATH), $(ASM_INT))
C_SRC = $(addprefix $(C_PATH), $(C_KERNEL))
FONT_SRC = $(addprefix $(FONT_PATH), $(FONT_FILE))

# Object Files Names (sans chemin)
OBJ_ASM_BOOT = $(ASM_BOOT:.s=.o)
OBJ_ASM = $(ASM_INT:.s=.o)
OBJ_C = $(C_KERNEL:.c=.o)
OBJ_FONT = $(FONT_FILE:.psf=.o)

# Full Object Paths (placés dans obj/)
ASM_BOOT_OBJ = $(addprefix $(OBJ_PATH), $(OBJ_ASM_BOOT))
ASM_OBJ = $(addprefix $(OBJ_PATH), $(OBJ_ASM))
C_OBJ = $(addprefix $(OBJ_PATH), $(OBJ_C))
FONT_OBJ = $(addprefix $(OBJ_PATH), $(OBJ_FONT))

# Output Binaries
KERNEL_BIN = $(OS_NAME)
ISO_IMAGE = github_$(OS_NAME)_iso/$(OS_NAME).iso
ISODIR = isodir

# ==========================================
# Build Rules
# ==========================================

.PHONY: all clean fclean

all: $(ISO_IMAGE)

# 1. Compile Assembly bootloader (GNU as)
$(OBJ_PATH)%.o: $(BOOT_PATH)%.s
	@mkdir -p $(OBJ_PATH)
	$(AS) -o $@ $<

# 1.bis Compile Interrupts Stubs (NASM)
$(OBJ_PATH)%.o: $(ASM_PATH)%.s
	@mkdir -p $(OBJ_PATH)
	$(NASM) $(NASM_FLAGS) -o $@ $<

# 1.ter Compile Font (objcopy)
$(OBJ_PATH)%.o: $(FONT_PATH)%.psf
	@mkdir -p $(OBJ_PATH)
	cd $(FONT_PATH) && objcopy -I binary -O elf32-i386 -B i386 $(notdir $<) ../$@

# 2. Compile C kernel
$(OBJ_PATH)%.o: $(C_PATH)%.c $(HEADERS)
	@mkdir -p $(OBJ_PATH)
	$(CC) -c $< -o $@ $(CFLAGS)

# 3. Link kernel binary (On inclut bien boot.o, interrupt.o, kernel.o et font.o)
$(KERNEL_BIN): $(ASM_BOOT_OBJ) $(ASM_OBJ) $(C_OBJ) $(FONT_OBJ)
	$(LD) $(LDFLAGS) -o $@ $^ -lgcc

# 4. Create ISO image
$(ISO_IMAGE): $(KERNEL_BIN)
	mkdir -p $(ISODIR)/boot/grub
	cp $(KERNEL_BIN) $(ISODIR)/boot/$(OS_NAME)
	grub-mkrescue -o $@ $(ISODIR)

# Clean up object files and kernel binary
clean:
	rm -rf $(OBJ_PATH) $(KERNEL_BIN)

# Thorough clean
fclean: clean
	rm -f $(ISO_IMAGE)
