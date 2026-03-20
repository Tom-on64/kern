#ifndef _BOOTLOADER_H
#define _BOOTLOADER_H

#include <kernel.h>
#include <limine.h>

struct bootloader {
	size_t hhdm_offset;
	size_t kernel_virt_base;
	size_t kernel_phys_base;
	size_t mm_total_memory;
	uint64_t mm_entry_count;
	LIMINE_PTR(struct limine_memmap_entry**) mm_entries;
	uint64_t fb_entry_count;
	LIMINE_PTR(struct limine_framebuffer**) fb_entries;
};

extern struct bootloader bootloader;

#define IS_INSIDE_HHDM(_addr) \
	((size_t)(_addr) >= bootloader.hhdm_offset &&	\
	 (size_t)(_addr) <= (bootloader.hhdm_offset + bootloader.mm_total))

int boot_init(void);

#endif
