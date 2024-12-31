# Tools
HOSTCC = cc
CC = i686-elf-gcc
LD = i686-elf-ld
AS = nasm
AR = i686-elf-ar
OBJCOPY = i686-elf-objcopy
MAKE = make

# Flags
HOSTCFLAGS = -Wall -Wextra -Wpedantic -O2 -std=c17 -g
CFLAGS = -Wall -Wextra -Wpedantic -Wno-pointer-arith\
	 -ffreestanding -fno-stack-protector -fno-builtin -fno-pie -fcommon\
	 -O2 -std=c17 -nostdinc -nostdlib -z notext -march=i386 -g
LDFLAGS = -melf_i386
ASFLAGS = 
ARFLAGS =

