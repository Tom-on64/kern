include ../defaults.mk

INC = ./include
BIN = ./bin

SRCS = $(shell find . -name '*.c')
OBJS = $(patsubst %.c, $(BIN)/%.o, $(SRCS))
OUT = ./libc.o

.PHONY: all clean

all: $(OUT)
$(OUT): $(OBJS)
	$(AR) $(ARFLAGS) rcs $@ $^

$(BIN)/%.o: %.c
	@mkdir -p $(BIN)/$(dir $<)
	$(CC) $(CFLAGS) -I$(INC) -c -o $@ $<

clean:
	rm -fr $(BIN) $(OUT)

