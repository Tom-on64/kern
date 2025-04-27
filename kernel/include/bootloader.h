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

#define BOOT_MAX_MMAP_ENTRIES	128

// A memory map entry
struct boot_memmap {
	uint64_t base;	// Memory region address
	uint64_t size;	// Memory region length
	uint8_t  type;	// Memory region type
} __packed;

/*
 * Some values may not be provided by the bootloader, in this case
 * pointers will be NULL and integer values will be 0 if applicable.
 * Some unprovided values may also be implied. Eg.:
 * 	If 'memmap = NULL' then 'memmap_size' doesn't matter. 

 */
struct boot_info {
	uint32_t mem_lower;
	uint32_t mem_upper;
	uint8_t  boot_drive;
	uint8_t  boot_part1;
	uint8_t  boot_part2;
	uint8_t  boot_part3;
	struct   boot_memmap* memmap;
	size_t   memmap_size;
	char*    cmdline;
	char*    bootname;
} __packed;
extern struct boot_info bootloader;

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
