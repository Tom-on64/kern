#ifndef _BOOTLOADER_H
#define _BOOTLOADER_H

#include <kernel.h>

// Boot protocols
#define BOOT_UNKNOWN	0	// Unknown
#define BOOT_MB1	1	// MultiBoot 1
#define BOOT_MB2	2	// MultiBoot 2
#define BOOT_LIMINE	3	// Limine

struct boot_memmapEntry {
	uint64_t base;
	uint64_t size;
	uint32_t type;
	uint32_t attr;
};

/*
 * Some values may not be provided by the bootloader, in this case
 * pointers will be NULL and integer values will be 0 if applicable.
 * Some unprovided values may also be implied. Eg.:
 * 	If 'memmap = NULL' then 'memmapLen' doesn't matter. 
 */
struct boot_info {
	uint32_t memLower;
	uint32_t memUpper;
	struct boot_memmapEntry* memmap;
	size_t memmapLen;
};

/*
 * This functions arguments depend on the boot protocol provided.
 * This is the reason for the varargs thing. It runs on of the functions
 * defined after it.
 */
int boot_init(int type, ...);

int boot_init_mb1(void* ptr, uint32_t magic);
// TODO:
// int boot_init_mb2(...);
// int boot_init_limine(...);

#endif
