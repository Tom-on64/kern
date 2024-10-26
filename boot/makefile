#
# Bootloader
#

include make.defaults

STAGE1 = $(BIN)/stage1.bin
STAGE2 = $(BIN)/stage2.bin
STAGE3 = $(BIN)/stage3.bin
STAGE1_SRC = $(BOOT_DIR)/stage1.s
STAGE2_SRC = $(BOOT_DIR)/stage2.s
STAGE3_SRC = $(BOOT_DIR)/stage3.c
STAGE3_OBJ = $(BIN)/boot/stage3.o

bootloader: $(STAGE1) $(STAGE2) $(STAGE3)

$(STAGE1): $(STAGE1_SRC)
	@echo "Assembling $<..."
	$(AS) -fbin -o $@ $< $(ASFLAGS)

$(STAGE2): $(STAGE2_SRC)
	@echo "Assembling $<..."
	$(AS) -fbin -o $@ $< $(ASFLAGS)

$(STAGE3): $(STAGE3_SRC)
	@echo "Compiling $(STAGE3_SRC)..."
	$(CC) $(CFLAGS) -I$(LIBK_INC) -I$(LIBC_INC) -o $(STAGE3_OBJ) -c $<
	$(LD) $(LDFLAGS) -T$(BOOT_DIR)/linker.ld -o $@ $(STAGE3_OBJ) $(LIBK)

