include config.mk

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
	cp $(LIMINE_CFG) $(SYSROOT)/boot/limine.conf
	qemu-img create $(TARGET) $(IMG_SIZE)M
	echo ',,c,*' | sfdisk $(TARGET) >/dev/null
	mformat -F -i $(TARGET)@@$(OFFSET) ::
	mcopy -i $(TARGET)@@$(OFFSET) $(SYSROOT)/* ::
	limine bios-install $(TARGET)
	
# Build out base directory structure
$(SYSROOT):
	@mkdir $(SYSROOT)
	@mkdir $(SYSROOT)/boot
	@mkdir $(SYSROOT)/dev
	@mkdir $(SYSROOT)/etc
	@mkdir $(SYSROOT)/usr
	@mkdir $(SYSROOT)/usr/bin
	@mkdir $(SYSROOT)/usr/include
	@mkdir $(SYSROOT)/usr/lib
	@mkdir $(SYSROOT)/usr/sbin
	@mkdir $(SYSROOT)/usr/share
	@mkdir $(SYSROOT)/tmp
	@ln -s usr/bin $(SYSROOT)/bin
	@ln -s usr/lib $(SYSROOT)/lib
	@ln -s usr/sbin $(SYSROOT)/sbin

$(MODULES):
	$(MAKE) -C $@ SYSROOT=$(abspath $(SYSROOT))

clean:
	rm -f $(TARGET)
	rm -fr $(SYSROOT)
	rm -f $(Q_LOGFILE)
	for m in $(MODULES); do $(MAKE) -C $$m clean; done

qemu:
	qemu-system-x86_64\
		-drive format=raw,file=$(TARGET),index=0,media=disk\
		-m $(Q_MEMORY) -accel tcg -monitor stdio -net none\
		-rtc base=localtime,clock=host,driftfix=slew\
		-audiodev $(Q_AUDIODEV),id=audio0 -machine pcspk-audiodev=audio0\
		-d $(Q_LOGGING) -D $(Q_LOGFILE)

