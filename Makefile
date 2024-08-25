# Variables
CC = i686-elf-gcc
LD = i686-elf-ld
AS = nasm

CFLAGS = -std=c17 -ffreestanding -fno-builtin -fno-stack-protector -fno-pie \
		 -Os -nostdinc -mgeneral-regs-only -m32 -march=i386 -Wall -Iinclude/c
LDFLAGS = -z notext -melf_i386

SRC = ./src
BIN = ./bin
C_FILES = calc edit
FONTS = testfont term16n

.PHONY: os clean run

# Main compilation target
os: $(BIN) bootloader kernel $(C_FILES) $(FONTS)
	@cc ./makedisk.c -o ./makedisk
	@./makedisk # TODO: List of files
	@echo "\nDone!\n"

$(BIN):
	@[ -d $(BIN) ] || mkdir $(BIN)
	@rm -rf $(BIN)/*

# Bootloader: This target is responsible for creating stage?.bin (1,2,3)
bootloader: $(SRC)/stage1.asm $(SRC)/stage2.asm
	@echo "Assembling $(SRC)/stage1.asm..."
	@$(AS) -fbin -o $(BIN)/stage1.bin $(SRC)/stage1.asm
	@echo "Assembling $(SRC)/stage2.asm..."
	@$(AS) -fbin -o $(BIN)/stage2.bin $(SRC)/stage2.asm
	@echo "Compiling $(SRC)/stage3.c..."
	@$(CC) $(CFLAGS) -Iinclude -o $(BIN)/stage3.o -c $(SRC)/stage3.c
	@$(LD) $(LDFLAGS) -T$(SRC)/stage3.ld --oformat binary -o $(BIN)/stage3.bin $(BIN)/stage3.o
	@rm $(BIN)/stage3.o

# Kernel: This target is responsible for creating kernel.bin
kernel: $(SRC)/kernel.c
	@echo "Compiling $(SRC)/kernel.c..."
	@$(CC) $(CFLAGS) -Iinclude -o $(BIN)/kernel.o -c $(SRC)/kernel.c
	@$(LD) $(LDFLAGS) -T$(SRC)/kernel.ld --oformat binary -o $(BIN)/kernel.bin $(BIN)/kernel.o
	@rm $(BIN)/kernel.o

# This target compiles *.c (except kernel.c)
$(C_FILES):
	@echo "Compiling $(SRC)/$@.c..."
	@$(CC) $(CFLAGS) -o $(BIN)/$@.o -c $(SRC)/$@.c 
	@$(LD) $(LDFLAGS) -T$(SRC)/$@.ld --oformat binary -o $(BIN)/$@.bin $(BIN)/$@.o
	@rm $(BIN)/$@.o

# This target builds all fonts inside src/fonts
$(FONTS):
	@cp $(SRC)/fonts/$@.fnt $(BIN)/$@.fnt

# Run QEMU
run:
	qemu-system-x86_64\
		-drive format=raw,file="./kern.iso",index=0,media=disk\
		-m 256M -accel tcg -monitor stdio\
		-rtc base=localtime,clock=host,driftfix=slew\
		-audiodev coreaudio,id=audio0 -machine pcspk-audiodev=audio0

# Clean all build files
clean:
	@rm -rf $(BIN)
	@#rm -f kern.iso
	@rm -f makedisk

