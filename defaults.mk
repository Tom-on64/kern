# Tools
HOSTCC = cc
CC = cc
LD = ld
AS = nasm
AR = ar
OBJCOPY = objcopy
MAKE = make

# Flags
HOSTCCFLAGS = -Wall -Wextra -Wpedantic -Os -std=c17
CCFLAGS = -Wall -Wextra -Wpedantic -Wno-pointer-arith\
	 -ffreestanding -fstack-protector -fno-builtin -fno-pie -fcommon\
	 -O2 -std=c17 -nostdinc -nostdlib -z notext -march=i386 -m32
LDFLAGS = -melf_i386
ASFLAGS = 
ARFLAGS =

