#
# Kernel library
#

include make.defaults

LIBK_ASM=$(LIBK_DIR)/cpu/isr.s

$(LIBK): $(LIBK_SRC) $(LIBK_ASM) $(LIBC)
	@echo "Building $(LIBK)..."
	nasm -felf32 -o $(BIN)/libk/isr_asm.o $(LIBK_ASM)
	for f in $(LIBK_SRC); do\
		filename=$$(basename $$f .c);\
		$(CC) $(CFLAGS) -I$(LIBC_INC) -I$(LIBK_INC) -o $(BIN)/libk/$$filename.o -c $$f;\
	done
	$(LD) $(LDFLAGS) -r -o $(LIBK) $(LIBC) $(BIN)/libk/**.o

