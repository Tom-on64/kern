include ../config.mk

SRC = ./src
BIN = ./bin
INC = ./include
SRCS_C = $(wildcard $(SRC)/*.c)
SRCS_S = $(wildcard $(SRC)/*.s)
OBJS_C = $(patsubst $(SRC)/%.c, $(BIN)/%.c.o, $(SRCS_C))
OBJS_S = $(patsubst $(SRC)/%.s, $(BIN)/%.s.o, $(SRCS_S))
OUT = $(SYSROOT)/boot/kernel.elf
LINKER = ./linker.ld

# Stack Canary
SSP := 0x$(shell openssl rand -hex 8)
CCFLAGS += -DSTACK_CHK_GUARD=$(SSP)

.PHONY: all clean
all: $(OUT)
$(OUT): $(OBJS_C) $(OBJS_S)
	$(LD) $(LDFLAGS) -T$(LINKER) -o $@ $^

$(BIN)/%.c.o: $(SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -I$(INC) -c -o $@ $^

$(BIN)/%.s.o: $(SRC)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $^

clean:
	rm -fr $(BIN) $(OUT)

