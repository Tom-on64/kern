include ../defaults.mk

INC = ./include
SRC = ./src
BIN = ./bin

SRCS_C = $(shell find $(SRC) -name '*.c')
SRCS_S = $(shell find $(SRC) -name '*.s')
OBJS = $(patsubst $(SRC)/%.c, $(BIN)/%.o, $(SRCS_C))	\
       $(patsubst $(SRC)/%.s, $(BIN)/%.s.o, $(SRCS_S))

LINKER = ./linker.ld
OUT = ./kern.bin

.PHONY: all clean

all: $(OUT)
$(OUT): $(OBJS)
	$(LD) $(LDFLAGS) -T$(LINKER) -o $@ $^

$(BIN)/%.o: $(SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INC) -c -o $@ $<

$(BIN)/%.s.o: $(SRC)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -felf32 -o $@ $<

clean:
	rm -fr $(BIN) $(OUT)

