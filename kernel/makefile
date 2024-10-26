#
# Kernel makefile
#

include make.defaults

$(KERNEL): $(KERNEL_SRC) $(LIBC) $(LIBK)
	@echo "Compiling $(KERNEL_SRC)..."
	$(CC) $(CFLAGS) -I$(LIBK_INC) -I$(LIBC_INC) -o $(BIN)/kernel/kernel.o -c $(KERNEL_SRC)
	$(LD) $(LDFLAGS) -T$(KERNEL_DIR)/linker.ld -o $@ $(BIN)/kernel/kernel.o $(LIBK)

