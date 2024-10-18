#
# Kernel library
#

include make.defaults

$(LIBK): $(LIBK_SRC)
	@echo "Building $(LIBK)..."
	for f in $(LIBK_SRC); do\
		filename=$$(basename $$f .c);\
		$(CC) $(CFLAGS) -I$(LIBC_INC) -I$(LIBK_INC) -o $(BIN)/libk/$$filename.o -c $$f;\
	done
	$(LD) $(LDFLAGS) -r -o $(LIBK) $(BIN)/libk/**.o

