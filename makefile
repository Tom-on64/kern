IMG = kern.img
BOOT = ./boot
KERNEL = ./kernel
LIBC = ./libc

MODULES = $(BOOT)\
	  $(KERNEL)\
	  $(LIBC)

.PHONY: all clean $(MODULES) qemu

all: $(IMG)
$(IMG): $(MODULES)
	dd if=/dev/zero of=$(IMG) bs=512 count=131072
	dd if=$(BOOT)/stage1.bin of=$(IMG) bs=512 seek=0 conv=notrunc
	dd if=$(BOOT)/stage2.bin of=$(IMG) bs=512 seek=1 conv=notrunc
	echo ',,b,*' | sfdisk $(IMG)
	echo 'drive A:\n\tfile="$(IMG)"\n\tpartition=1' > ~/.mtoolsrc
	mformat -F 'A:'
	mcopy $(KERNEL)/kern.bin 'A:'

$(MODULES):
	$(MAKE) -C $@

clean:
	rm -f $(IMG)
	for m in $(MODULES); do $(MAKE) -C $$m clean; done

qemu:
	qemu-system-x86_64\
		-drive format=raw,file=$(IMG),index=0,media=disk\
		-m 256M -accel tcg -monitor stdio\
		-rtc base=localtime,clock=host,driftfix=slew\
		-audiodev coreaudio,id=audio0 -machine pcspk-audiodev=audio0

