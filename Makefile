# Variables
CC = i386-elf-gcc
LD = i386-elf-ld
AS = nasm

CFLAGS = -ffreestanding -fno-builtin -fno-stack-protector -nostdinc -mgeneral-regs-only -m32 -march=i386 -Wall -Iinclude/c

SRC = ./src
BUILD = ./build
C_FILES = calc
FONTS = testfont term16n

.PHONY: os clean run

# Main compilation target
os: $(BUILD) filesystem
	@echo "\nDone!\n"

# Filesystem
filesystem: bootloader kernel $(C_FILES) $(FONTS)
	@echo "Building filesystem..."
	@$(AS) -fbin -o $(BUILD)/filetable.bin $(SRC)/filetable.asm
	@dd if=/dev/zero of=kern.iso bs=512 count=2880 status=none
# TODO: Make this not so manual
	@dd if=$(BUILD)/boot.bin      of=kern.iso bs=512 seek=0  conv=notrunc status=none
	@dd if=$(BUILD)/filetable.bin of=kern.iso bs=512 seek=5  conv=notrunc status=none
	@dd if=$(BUILD)/kernel.bin    of=kern.iso bs=512 seek=6  conv=notrunc status=none
	@dd if=$(BUILD)/term16n.bin   of=kern.iso bs=512 seek=46 conv=notrunc status=none
	@dd if=$(BUILD)/testfont.bin  of=kern.iso bs=512 seek=50 conv=notrunc status=none
	@dd if=$(SRC)/fs/test.txt     of=kern.iso bs=512 seek=54 conv=notrunc status=none
	@dd if=$(BUILD)/calc.bin      of=kern.iso bs=512 seek=55 conv=notrunc status=none

$(BUILD):
	@[ -d $(BUILD) ] || mkdir $(BUILD)
	@rm -rf $(BUILD)/*

# Bootloader: This target is responsible for creating boot.bin
bootloader: $(SRC)/stage1.asm $(SRC)/stage2.asm
	@echo "Assembling $(SRC)/stage1.asm..."
	@$(AS) -fbin -o $(BUILD)/stage1.bin $(SRC)/stage1.asm
	@echo "Assembling $(SRC)/stage2.asm..."
	@$(AS) -fbin -o $(BUILD)/stage2.bin $(SRC)/stage2.asm
	@cat $(BUILD)/stage1.bin $(BUILD)/stage2.bin > $(BUILD)/boot.bin

# Kernel: This target is responsible for creating kernel.bin
kernel: $(SRC)/kernel.c $(SRC)/entry.asm
	@echo "Assembling $(SRC)/entry.asm..."
	@$(AS) -felf32 -o $(BUILD)/entry.o $(SRC)/entry.asm
	@echo "Compiling $(SRC)/kernel.c..."
	@$(CC) $(CFLAGS) -Iinclude -o $(BUILD)/kernel.o -c $(SRC)/kernel.c
	@$(LD) -T$(SRC)/kernel.ld --oformat binary -o $(BUILD)/kernel.bin $(BUILD)/*.o
	@rm $(BUILD)/*.o

$(C_FILES):
	@echo "Compiling $(SRC)/$@.c..."
	@$(CC) $(CFLAGS) -o $(BUILD)/$@.o -c $(SRC)/$@.c 
	@$(LD) -T$(SRC)/$@.ld --oformat binary -o $(BUILD)/$@.bin $(BUILD)/$@.o
	@rm $(BUILD)/$@.o

$(FONTS):
	@echo "Assembling $(SRC)/fonts/$@.asm..."
	@$(AS) -fbin -o $(BUILD)/$@.bin $(SRC)/fonts/$@.asm

# Run QEMU
run:
	qemu-system-x86_64\
		-drive format=raw,file="./kern.iso",index=0,media=disk\
		-m 256M\
		-accel tcg\
		-rtc base=localtime,clock=host,driftfix=slew\
		-audiodev coreaudio,id=audio0 -machine pcspk-audiodev=audio0

# Clean all build files
clean:
	@rm -rf $(BUILD)
	@rm -f kern.iso

