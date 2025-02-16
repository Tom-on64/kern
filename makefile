KERNEL = ./kernel

MODULES = $(KERNEL)

.PHONY: all clean $(MODULES) qemu

all: $(MODULES)
$(MODULES):
	$(MAKE) -C $@

clean:
	rm -f $(IMG)
	for m in $(MODULES); do $(MAKE) -C $$m clean; done

qemu:
	qemu-system-x86_64\
		-drive format=raw,file=$(IMG),index=0,media=disk\
		-m 256M -accel tcg -monitor stdio -net none\
		-rtc base=localtime,clock=host,driftfix=slew\
		-audiodev pipewire,id=audio0 -machine pcspk-audiodev=audio0

