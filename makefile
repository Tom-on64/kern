include defaults.mk

TARGET = ./$(IMG_NAME)
OFFSET = 1048576
SYSROOT = ./sysroot

# Modules
KERNEL = ./kernel
MODULES = $(KERNEL)

# Treat these targets as names and not files
.PHONY: all clean $(MODULES) qemu

all: $(TARGET)
$(TARGET): $(SYSROOT) $(MODULES)
	cp /usr/share/limine/limine-bios.sys $(SYSROOT)/boot
	echo -e 'timeout: 0\n\n/kern.\n\tprotocol: multiboot1\n\tpath: boot():/boot/kernel.elf\n' > $(SYSROOT)/boot/limine.conf
	dd if=/dev/zero of=$(TARGET) bs=1M count=$(IMG_SIZE)
	echo ',,c,*' | sfdisk $(TARGET) >/dev/null
	mformat -F -i $(TARGET)@@$(OFFSET) ::
	mcopy -i $(TARGET)@@$(OFFSET) $(SYSROOT)/* ::
	limine bios-install $(TARGET)
	
# Build out base directory structure
$(SYSROOT):
	@mkdir $(SYSROOT)
	@mkdir $(SYSROOT)/bin
	@mkdir $(SYSROOT)/boot
	@mkdir $(SYSROOT)/dev
	@mkdir $(SYSROOT)/etc
	@mkdir $(SYSROOT)/lib
	@mkdir $(SYSROOT)/sbin
	@mkdir $(SYSROOT)/usr
	@mkdir $(SYSROOT)/usr/bin
	@mkdir $(SYSROOT)/usr/include
	@mkdir $(SYSROOT)/usr/share
	@mkdir $(SYSROOT)/tmp

$(MODULES):
	$(MAKE) -C $@ SYSROOT=$(abspath $(SYSROOT))

clean:
	rm -f $(TARGET)
	rm -fr $(SYSROOT)
	for m in $(MODULES); do $(MAKE) -C $$m clean; done

qemu:
	qemu-system-x86_64\
		-drive format=raw,file=$(TARGET),index=0,media=disk\
		-m 256M -accel tcg -monitor stdio -net none\
		-rtc base=localtime,clock=host,driftfix=slew\
		-audiodev $(AUDIODEV),id=audio0 -machine pcspk-audiodev=audio0

