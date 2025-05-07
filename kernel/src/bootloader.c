#include <bootloader.h>
#include <multiboot.h>
#include <kernel.h>
#include <string.h>

struct boot_memmap memmap[BOOT_MAX_MMAP_ENTRIES];
struct boot_info bootloader;

// TODO:
int boot_init(int type, ...) {
	va_list va;
	va_start(va, type);

	void* ptr = va_arg(va, void*);
	uint32_t magic = va_arg(va, uint32_t);

	switch (type) {
	case BOOT_MB1: return boot_init_mb1(ptr, magic);
	}
	
	return 1;
}

int boot_init_mb1(void* ptr, uint32_t magic) {
	multiboot_info_t* mbi = ptr;

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) return 1;

	// mem_* fields are valid
	if (mbi->flags & MULTIBOOT_INFO_MEMORY) {
		bootloader.mem_lower = mbi->mem_lower;
		bootloader.mem_upper = mbi->mem_upper;
	}

	// boot_device is valid
	if (mbi->flags & MULTIBOOT_INFO_BOOTDEV) {
		bootloader.boot_drive = (mbi->boot_device & 0xff000000) >> 24;
		bootloader.boot_part1 = (mbi->boot_device & 0x00ff0000) >> 16;
		bootloader.boot_part2 = (mbi->boot_device & 0x0000ff00) >> 8;
		bootloader.boot_part3 = (mbi->boot_device & 0x000000ff);
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
		bootloader.memmap = &memmap[0];

		size_t i = 0;
		multiboot_memory_map_t* entry = (multiboot_memory_map_t*)mbi->mmap_addr;
		uint32_t limit = mbi->mmap_addr + mbi->mmap_length;

		while ((uint32_t)entry < limit) {
			bootloader.memmap[i].base = entry->addr;
			bootloader.memmap[i].size = entry->len;
			bootloader.memmap[i].type = (uint8_t)entry->type;

			entry++;
			i++;
		}
		bootloader.memmap_size = i;
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
	
	debugf("[boot] Initiated with MultiBoot 1\n");

	return 0;
}

