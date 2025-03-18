#include <bootloader.h>
#include <multiboot.h>
#include <kernel.h>

struct boot_info bootloader = { 0 };

int boot_init(int type, ...) {
	va_list args;
	va_start(args, type);

	switch (type) {
	case BOOT_UNKNOWN: 	return 1;
	case BOOT_MB1:		return boot_init_mb1(va_arg(args, void*), va_arg(args, uint32_t));
	case BOOT_MB2:		return 1;
	case BOOT_LIMINE:	return 1;
	}

	// We got an invalid boot protocol
	return 1;
}

int boot_init_mb1(void* ptr, uint32_t magic) {
	multiboot_info_t* mbi = ptr;

	if (magic != 0xBADB002) return 1;

	// Memory info
	if (mbi->flags & MULTIBOOT_INFO_MEMORY) {
		bootloader.memLower = mbi->mem_lower;
		bootloader.memUpper = mbi->mem_upper;
	}

	// Memory map
	if (mbi->flags & MULTIBOOT_INFO_MEM_MAP) {
		uint32_t memmap = mbi->mmap_addr;
		uint32_t offset = 0;
		while (offset < mbi->mmap_length) {
			multiboot_memory_map_t* entry = (multiboot_memory_map_t*)(memmap + offset);
			offset += entry->size;

			
		}
	}

	return 0;
}

