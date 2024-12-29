include defaults.mk

SRCS = $(shell find $(SRC) -name '**.c')
OBJS = $(patsubst $(SRC)/%.c, $(BIN)/%.o, $(SRCS))
LINKER = $(SRC)/linker.ld
OUT = $(BIN)/kernel.elf

.PHONY: all clean

all: $(OUT)
$(OUT): $(OBJS)
	$(LD) $(LDFLAGS) -T $(LINKER) -o $@ $^

$(BIN)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -fr $(BIN)

