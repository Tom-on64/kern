# Variables
CC = i386-elf-gcc
LD = i386-elf-ld
AS = nasm

CFLAGS = -ffreestanding -m32 -Iinclude -Wall -Wextra

SRC = ./src
BUILD = ./build
# C_FILES = $(shell find $(SRC) -name "*.c" -exec basename -s ".c" {} \;)

.PHONY: os clean run

# Main compilation target
os: $(BUILD) bootloader kernel
	@$(AS) -fbin -o $(BUILD)/filetable.bin $(SRC)/filetable.asm
	@$(AS) -fbin -o $(BUILD)/testfont.bin $(SRC)/testfont.asm
	@cat $(BUILD)/boot.bin $(BUILD)/filetable.bin $(BUILD)/kernel.bin > $(BUILD)/os.bin
	@dd if=/dev/zero of=kern.iso bs=512 count=2880
	@dd if=$(BUILD)/os.bin of=kern.iso conv=notrunc
	@dd if=$(BUILD)/testfont.bin of=kern.iso bs=512 seek=25 conv=notrunc
	@dd if=$(SRC)/fs/test.txt of=kern.iso bs=512 seek=29 conv=notrunc

$(BUILD):
	@[ -d $(BUILD) ] || mkdir $(BUILD)
	@rm -rf $(BUILD)/*

# Bootloader: This target is responsible for creating boot.bin
bootloader:
	@$(AS) -fbin -o $(BUILD)/stage1.bin $(SRC)/stage1.asm
	@$(AS) -fbin -o $(BUILD)/stage2.bin $(SRC)/stage2.asm
	@cat $(BUILD)/stage1.bin $(BUILD)/stage2.bin > $(BUILD)/boot.bin

# Kernel: This target is responsible for creating kernel.bin
kernel: $(SRC)/kernel.c $(SRC)/entry.asm
	@$(AS) -felf32 -o $(BUILD)/entry.o $(SRC)/entry.asm
	@$(CC) $(CFLAGS) -o $(BUILD)/kernel.o -c $(SRC)/kernel.c
	@$(LD) -T $(SRC)/kernel.ld --oformat binary -o $(BUILD)/kernel.bin $(BUILD)/*.o

# Run QEMU
run:
	qemu-system-x86_64 -drive format=raw,file="./kern.iso",index=0,media=disk -m 256M -accel tcg

# Clean all build files
clean:
	@rm -rf $(BUILD)
	@rm -f kern.iso

