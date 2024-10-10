# Variables
CC := i686-elf-gcc
LD := i686-elf-ld
AS := nasm

CCFLAGS := -g -std=c17 -ffreestanding -fno-builtin -fno-stack-protector -fno-pie \
		 -Os -nostdinc -mgeneral-regs-only -m32 -march=i386 -Wall
LDFLAGS := -z notext -melf_i386 -nostdlib
ASFLAGS :=

include make.config

SRC := ./src
BIN := ./bin
LIBK_INC := ./libk/include
LIBC_INC := ./libc/include
LIBC_DIR := ./libc
LIBK_DIR := ./libk
LIBK_SRC := $(shell find $(LIBK_DIR) -type f -name "**.c")

FONTS := testfont term16n

BOOTLOADER := $(BIN)/stage1.bin $(BIN)/stage2.bin $(BIN)/stage3.bin
KERNEL := $(BIN)/kernel.bin
LIBC := $(BIN)/libc.o
LIBK := $(BIN)/libk.o
ISO := kern.iso

.PHONY: os clean run

# Main compilation target
os: $(ISO)
$(ISO): $(BIN) $(BOOTLOADER) $(KERNEL) $(C_FILES) $(FONTS)
	cc ./makedisk.c -o ./makedisk
	./makedisk # TODO: List of files
	@echo "\nDone!\n"

$(BIN):
	[ -d $(BIN) ] || mkdir $(BIN)
	rm -rf $(BIN)/*

# Bootloader: This target is responsible for creating stage?.bin (1,2,3)
$(BOOTLOADER): $(SRC)/stage1.asm $(SRC)/stage2.asm $(SRC)/stage3.c $(LIBC) $(LIBK)
	@echo "Assembling $(SRC)/stage1.asm..."
	$(AS) -fbin -o $(BIN)/stage1.bin $(SRC)/stage1.asm $(ASFLAGS) 
	@echo "Assembling $(SRC)/stage2.asm..."
	$(AS) -fbin -o $(BIN)/stage2.bin $(SRC)/stage2.asm $(ASFLAGS) 
	@echo "Compiling $(SRC)/stage3.c..."
	$(CC) $(CCFLAGS) -I$(LIBK_INC) -I$(LIBC_INC) -o $(BIN)/stage3.o -c $(SRC)/stage3.c
	$(LD) $(LDFLAGS) -T$(SRC)/stage3.ld --oformat binary -o $(BIN)/stage3.bin $(BIN)/stage3.o $(LIBC) $(LIBK)
	rm $(BIN)/stage3.o

# Kernel: This target is responsible for creating kernel.bin
$(KERNEL): $(SRC)/kernel.c $(LIBC) #TODO: $(LIBK)
	@echo "Compiling $(SRC)/kernel.c..."
	$(CC) $(CCFLAGS) -I$(LIBK_INC) -I$(LIBC_INC) -o $(BIN)/kernel.o -c $(SRC)/kernel.c
	$(LD) $(LDFLAGS) -T$(SRC)/kernel.ld --oformat binary -o $(BIN)/kernel.bin $(BIN)/kernel.o $(LIBC) $(LIBK)
	rm $(BIN)/kernel.o

# TODO: user programs

$(LIBC): $(LIBC_DIR)/**.c
	@echo "Compiling $(LIBC)..."
	mkdir -p $(BIN)/libc
	for f in libc/*.c; do\
		filename=$$(basename $$f .c);\
		$(CC) $(CCFLAGS) -I$(LIBC_INC) -o $(BIN)/libc/$$filename.o -c $$f;\
	done
	$(LD) $(LDFLAGS) -r -o $(LIBC) $(BIN)/libc/**.o

$(LIBK): $(LIBK_SRC)
	@echo "Compiling $(LIBK)..."
	mkdir -p $(BIN)/libk
	for f in $(LIBK_SRC); do\
		filename=$$(basename $$f .c);\
		$(CC) $(CCFLAGS) -I$(LIBC_INC) -I$(LIBK_INC) -o $(BIN)/libk/$$filename.o -c $$f;\
	done
	$(LD) $(LDFLAGS) -r -o $@ $(BIN)/libk/**.o


# This target retrieves all fonts inside src/fonts
$(FONTS):
	cp $(SRC)/fonts/$@.fnt $(BIN)/$@.fnt

# Run QEMU
run:
	qemu-system-x86_64\
		-drive format=raw,file=$(ISO),index=0,media=disk\
		-m 256M -accel tcg -monitor stdio\
		-rtc base=localtime,clock=host,driftfix=slew\
		-audiodev coreaudio,id=audio0 -machine pcspk-audiodev=audio0

# Clean all build files
clean:
	rm -f kern.iso
	rm -rf $(BIN)
	rm -f makedisk

