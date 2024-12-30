include ../defaults.mk

INC = ./include
BIN = ./bin

SRCS_C = $(shell find . -name '*.c')
SRCS_S = $(shell find . -name '*.s')
OBJS = $(patsubst %.c, $(BIN)/%.o, $(SRCS_C))	\
       $(patsubst %.s, $(BIN)/%.s.o, $(SRCS_S))
LINKER = ./linker.ld
OUT = ./kernel.elf

.PHONY: all clean

all: $(OUT)
$(OUT): $(OBJS)
	$(LD) $(LDFLAGS) -T$(LINKER) -o $@ $^

$(BIN)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INC) -c -o $@ $<

$(BIN)/%.s.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -felf32 -o $@ $<

clean:
	rm -fr $(BIN) $(OUT)

