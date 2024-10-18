#
# C Standard library
#

include make.defaults

$(LIBC): $(LIBC_SRC)
	@echo "Building $(LIBC)..."
	for f in $(LIBC_SRC); do\
		filename=$$(basename $$f .c);\
		$(CC) $(CFLAGS) -I$(LIBC_INC) -o $(BIN)/libc/$$filename.o -c $$f;\
	done
	$(LD) $(LDFLAGS) -r -o $(LIBC) $(BIN)/libc/**.o

