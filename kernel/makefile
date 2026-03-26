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
SSP = ./.ssp

.PHONY: all clean
all: $(OUT)
$(OUT): $(OBJS_C) $(OBJS_S)
	$(LD) $(LDFLAGS) -T$(LINKER) -o $@ $^

$(BIN)/%.c.o: $(SRC)/%.c $(SSP)
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -DCONSOLE_LOGLEVEL='$(CONSOLE_LOGLEVEL)' -DSTACK_CHK_GUARD="$$(cat $(SSP))" -I$(INC) -c -o $@ $<

$(BIN)/%.s.o: $(SRC)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<

$(SSP):
	echo "0x$$(openssl rand -hex 8)UL" > $(SSP)

clean:
	rm -fr $(BIN) $(OUT) $(SSP)

