#include <bootloader.h>
#include <multiboot.h>
#include <kernel.h>

struct boot_info bootloader;

// TODO:
int boot_init(int type, ...) {
	return 1;
}

int boot_initMB1(void* ptr, uint32_t magic) {
	multiboot_info_t* mbi = ptr;

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) return 1;

	// mem_* fields are valid
	if (mbi->flags & MULTIBOOT_INFO_MEMORY) {
		bootloader.memLower = mbi->mem_lower;
		bootloader.memUpper = mbi->mem_upper;
	}

	// boot_device is valid
	if (mbi->flags & MULTIBOOT_INFO_BOOTDEV) {
		bootloader.bootDrive = (mbi->boot_device & 0xff000000) >> 24;
		bootloader.bootPart1 = (mbi->boot_device & 0x00ff0000) >> 16;
		bootloader.bootPart2 = (mbi->boot_device & 0x0000ff00) >> 8;
		bootloader.bootPart3 = (mbi->boot_device & 0x000000ff);
	}

	// cmdline is valid
	if (mbi->flags & MULTIBOOT_INFO_CMDLINE) {
		bootloader.cmdline = (char*)mbi->cmdline;
	}

	// TODO: mods
	// TODO: a.out symbol table
	// TODO: ELF symbol table
	
	// mmap_* fields are valid
	if (mbi->flags & MULTIBOOT_INFO_MEM_MAP) {
		bootloader.memmap = mbi->mmap_addr;
		bootloader.memmapLen = mbi->mmap_length;
	}

	// TODO: drives_*
	// TODO: config_table

	// boot_loader_name is valid
	if (mbi->flags & MULTIBOOT_INFO_BOOT_LOADER_NAME) {
		bootloader.bootname = (char*)mbi->boot_loader_name;
	}

	// TODO: APM table
	// TODO: VBE table
	// TODO: Framebuffer

	return 0;
}

