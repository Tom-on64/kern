#
# Root kern. makefile
# (c) 2023-2025 Tomas Kubecek
#

export VERSION = 0.3.0

# Configurable variables
include defaults.mk

BIN = ./bin
IMG = ./kern.img

.PHONY: all clean qemu msg $(MODULES)

all: msg $(IMG)
$(IMG): $(MODULES)
	@echo "\nDone!\n"

# Build each module
$(MODULES): 
	mkdir -p $(BIN)/$@
	$(MAKE) BIN=$(BIN)/$@ SRC=$@ -f $@/makefile all

clean:
	for m in $(MODULES); do $(MAKE) BIN=$(BIN)/$$m SRC=$$m -C . -f $$m/makefile clean; done
	rm -fr $(BIN)
	rm -fr $(IMG)

msg:
	@echo "\x1b[36mBuilding \x1b[1mkern.\x1b[22m V$(VERSION)\x1b[0m\n"

# Core Audio only works on MacOS
qemu:
	qemu-system-x86_64\
		-drive format=raw,file=$(IMG),index=0,media=disk\
		-m 256M -accel tcg -monitor stdio\
		-rtc base=localtime,clock=host,driftfix=slew\
		-audiodev coreaudio,id=audio0 -machine pcspk-audiodev=audio0

