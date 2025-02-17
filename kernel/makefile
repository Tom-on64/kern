include ../defaults.mk

SRC = ./src
BIN = ./bin
INC = ./include
SRCS_C = $(wildcard $(SRC)/*.c)
SRCS_S = $(wildcard $(SRC)/*.s)
OBJS_C = $(patsubst $(SRC)/%.c, $(BIN)/%.c.o, $(SRCS_C))
OBJS_S = $(patsubst $(SRC)/%.s, $(BIN)/%.s.o, $(SRCS_S))
OUT = ./kernel.elf
LINKER = ./linker.ld

.PHONY: all clean
all: $(OUT)
$(OUT): $(OBJS_C) $(OBJS_S)
	$(LD) $(LDFLAGS) -T$(LINKER) -o $@ $^

$(BIN)/%.c.o: $(SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -I$(INC) -c -o $@ $^

$(BIN)/%.s.o: $(SRC)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -felf32 -o $@ $^

clean:
	rm -fr $(BIN) $(OUT)

