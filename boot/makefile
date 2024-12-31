include ../defaults.mk

BIN = ./bin
STAGE1_SRC = ./stage1
STAGE2_SRC = ./stage2

STAGE1_SRCS = $(wildcard $(STAGE1_SRC)/*.s)
STAGE2_SRCS = $(wildcard $(STAGE2_SRC)/*.c)
STAGE2_OBJS = $(patsubst $(STAGE2_SRC)/%.c, $(BIN)/%.o, $(STAGE2_SRCS))

LINKER = $(STAGE2_SRC)/linker.ld
STAGE1 = stage1.bin
STAGE2 = stage2.bin

.PHONY: all clean

all: $(STAGE1) $(STAGE2)

$(STAGE1): $(STAGE1_SRCS)
	$(AS) $(ASFLAGS) -fbin -o $@ $(STAGE1_SRC)/bootsect.s

$(STAGE2): $(STAGE2_OBJS)
	$(LD) $(LDFLAGS) -T$(LINKER) -o $@ $^

$(BIN)/%.o: $(STAGE2_SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -fr $(BIN) $(STAGE1) $(STAGE2)

