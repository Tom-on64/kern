#ifndef _BOOTLOADER_H
#define _BOOTLOADER_H

#include <kernel.h>

// Boot protocols
#define BOOT_UNKNOWN	0	// Unknown
#define BOOT_MB1	1	// MultiBoot 1
#define BOOT_MB2	2	// MultiBoot 2
#define BOOT_LIMINE	3	// Limine

// Memmap region types
#define BOOT_MMAP_AVAILABLE	1
#define BOOT_MMAP_RESERVED	2
#define BOOT_MMAP_ACPI_RECLAIM	3
#define BOOT_MMAP_NVS		4
#define BOOT_MMAP_BADRAM	5

// A memory map entry
struct boot_memmap {
	uint32_t skip;	// Used for skipping (may be padded for alignment)
	uint64_t base;	// Memory region address
	uint64_t size;	// Memory region length
	uint32_t type;	// Memory region type
} __packed;

/*
 * Some values may not be provided by the bootloader, in this case
 * pointers will be NULL and integer values will be 0 if applicable.
 * Some unprovided values may also be implied. Eg.:
 * 	If 'memmap = NULL' then 'memmapLen' doesn't matter. 

 */
struct boot_info {
	uint32_t memLower;
	uint32_t memUpper;
	uint8_t bootDrive;
	uint8_t bootPart1;
	uint8_t bootPart2;
	uint8_t bootPart3;
	char* cmdline;
	uint32_t memmap;
	size_t memmapLen;
	char* bootname;
} __packed;
extern struct boot_info bootloader;

/*
 * This functions arguments depend on the boot protocol provided.
 * This is the reason for the varargs thing. It runs on of the functions
 * defined after it.
 */
int boot_init(int type, ...);

int boot_initMB1(void* ptr, uint32_t magic);
// TODO:
// int boot_initMB2(...);
// int boot_initLimine(...);

#endif
