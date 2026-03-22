# Tools
HOSTCC = cc
CC = cc
LD = ld
AS = nasm
AR = ar
OBJCOPY = objcopy
MAKE = make

# Flags
HOSTCCFLAGS = -Wall -Wextra -O2
CCFLAGS = -Wall -Wextra -m64 -ffreestanding -fcommon -Iinclude \
	  -mcmodel=kernel \
	  -fno-pie \
	  -fno-stack-protector \
	  -mno-mmx \
	  -mno-80387 \
	  -mno-red-zone \
	  -nostartfiles \
	  -nostdlib \
	  -nostdinc \
	  -fno-lto
ASFLAGS = -felf64
LDFLAGS = -melf_x86_64 -nostdlib -static \
	  --no-dynamic-linker \
	  -z text -z max-page-size=0x1000
ARFLAGS =

# Disk image
IMG_NAME = disk.img
IMG_SIZE = 256	# 256MB

# Qemu
Q_MEMORY = 256M
Q_AUDIODEV = pipewire
Q_LOGGING = int,cpu_reset
Q_LOGFILE = qemu.log

