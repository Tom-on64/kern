# Variables
CC = i386-elf-gcc
LD = i386-elf-ld
AS = nasm

CFLAGS = -std=c17 -ffreestanding -fno-builtin -fno-stack-protector -fno-pie \
		 -Os -nostdinc -mgeneral-regs-only -m32 -march=i386 -Wall -Iinclude/c

SRC = ./src
BUILD = ./build
C_FILES = calc editor
FONTS = testfont term16n

.PHONY: os clean run

# Main compilation target
os: $(BUILD) bootloader kernel $(C_FILES) $(FONTS)
	@cc ./makedisk.c -o ./makedisk
	@./makedisk # TODO: List of files
	@echo "\nDone!\n"

$(BUILD):
	@[ -d $(BUILD) ] || mkdir $(BUILD)
	@rm -rf $(BUILD)/*

# Bootloader: This target is responsible for creating boot.bin
bootloader: $(SRC)/stage1.asm $(SRC)/stage2.asm
	@echo "Assembling $(SRC)/stage1.asm..."
	@$(AS) -fbin -o $(BUILD)/stage1.bin $(SRC)/stage1.asm
	@echo "Assembling $(SRC)/stage2.asm..."
	@$(AS) -fbin -o $(BUILD)/stage2.bin $(SRC)/stage2.asm
	@echo "Compiling $(SRC)/stage3.c..."
	@$(CC) $(CFLAGS) -Iinclude -o $(BUILD)/stage3.o -c $(SRC)/stage3.c
	@$(LD) -T$(SRC)/stage3.ld -z notext --oformat binary -o $(BUILD)/stage3.bin $(BUILD)/stage3.o
	@cat $(BUILD)/stage1.bin $(BUILD)/stage2.bin $(BUILD)/stage3.bin > $(BUILD)/boot.bin
	@rm $(BUILD)/stage3.o

# Kernel: This target is responsible for creating kernel.bin
kernel: $(SRC)/kernel.c
	@echo "Compiling $(SRC)/kernel.c..."
	@$(CC) $(CFLAGS) -Iinclude -o $(BUILD)/kernel.o -c $(SRC)/kernel.c
	@$(LD) -T$(SRC)/kernel.ld -z notext --oformat binary -o $(BUILD)/kernel.bin $(BUILD)/kernel.o
	@rm $(BUILD)/kernel.o

# This target compiles *.c (except kernel.c)
$(C_FILES):
	@echo "Compiling $(SRC)/$@.c..."
	@$(CC) $(CFLAGS) -o $(BUILD)/$@.o -c $(SRC)/$@.c 
	@$(LD) -T$(SRC)/$@.ld -z notext --oformat binary -o $(BUILD)/$@.bin $(BUILD)/$@.o
	@rm $(BUILD)/$@.o

# This target builds all fonts inside src/fonts
$(FONTS):
	@echo "Assembling $(SRC)/fonts/$@.asm..."
	@$(AS) -fbin -o $(BUILD)/$@.fnt $(SRC)/fonts/$@.asm

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

