#
# Bootloader
#

include make.defaults

bootloader: $(BOOT_DIR)/stage1.asm $(BOOT_DIR)/stage2.asm $(BOOT_DIR)/stage3.c $(LIBC) $(LIBK)
	@echo "Assembling $(BOOT_DIR)/stage1.asm..."
	$(AS) -fbin -o $(BIN)/stage1.bin $(BOOT_DIR)/stage1.asm $(ASFLAGS) 
	@echo "Assembling $(BOOT_DIR)/stage2.asm..."
	$(AS) -fbin -o $(BIN)/stage2.bin $(BOOT_DIR)/stage2.asm $(ASFLAGS) 
	@echo "Compiling $(BOOT_DIR)/stage3.c..."
	$(CC) $(CFLAGS) -I$(LIBK_INC) -I$(LIBC_INC) -o $(BIN)/boot/stage3.o -c $(BOOT_DIR)/stage3.c
	$(LD) $(LDFLAGS) -T$(BOOT_DIR)/stage3.ld --oformat binary -o $(BIN)/stage3.bin $(BIN)/boot/stage3.o $(LIBC) $(LIBK)

