#
# Main kern makefile
#

export VERSION = 0.2.2

# Configurable variables (eg. CC, CFLAGS, ...)
include make.defaults

# The directories containing other makefiles
SUBDIRS = libc libk boot kernel

all: msg $(IMG)
msg:
	@echo\
		"\x1b[36mBuilding \x1b[1mkern.\x1b[22m V$(VERSION)\x1b[0m\n\x1b[32m (c) Tom-on 2024\x1b[0m\n"

# Build the final disk image
$(IMG): $(SUBDIRS) $(FONTS)
	$(HOSTCC) ./makedisk.c -I$(LIBK_INC)/fs -o ./makedisk
	./makedisk
	@echo "\n\x1b[32mDone!\x1b[0m\n"

$(SUBDIRS):
	mkdir -p $(BIN)/$@
	$(MAKE) -C . -f $@/makefile

$(FONTS):
	cp $(FONT_DIR)/$@.fnt $(BIN)/$@.fnt

.PHONY: all $(SUBDIRS) fonts clean qemu

clean:
	rm -rf $(BIN)
	rm -rf $(IMG)
	rm -f makedisk

qemu:
	qemu-system-x86_64\
		-drive format=raw,file=$(IMG),index=0,media=disk\
		-m 256M -accel tcg -monitor stdio\
		-rtc base=localtime,clock=host,driftfix=slew\
		-audiodev coreaudio,id=audio0 -machine pcspk-audiodev=audio0

