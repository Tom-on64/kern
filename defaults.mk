#
# kern. makefile defaults
#

# Tools
HOSTCC = cc
CC = i686-elf-gcc
LD = i686-elf-ld
AS = nasm
MAKE = make

# Flags
CFLAGS 	= -Wall -Wextra -Wpedantic -Wno-pointer-arith\
	  -ffreestanding -fno-stack-protector -fno-builtin -fno-pie -fcommon\
	  -O2 -std=c17 -nostdinc -nostdlib -z notext -march=i386
LDFLAGS	= -melf_i386
ASFLAGS	=

# Directories
KERNEL_DIR = ./kernel
BOOT_DIR = ./boot
LIBC_DIR = ./libc
USER_DIR = ./user

# Modules
MODULES	= $(KERNEL_DIR)	\
	  $(BOOT_DIR)	\
	  $(LIBC_DIR) 	\
	  $(USER_DIR)

