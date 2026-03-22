#ifndef _BOOTLOADER_H
#define _BOOTLOADER_H

#include <kernel.h>
#include <limine.h>

struct bootloader {
	offset_t hhdm_offset;
	uintptr_t kernel_virt_base;
	intptr_t kernel_phys_base;
	size_t mm_total_memory;
	size_t mm_entry_count;
	LIMINE_PTR(struct limine_memmap_entry**) mm_entries;
	size_t fb_entry_count;
	LIMINE_PTR(struct limine_framebuffer**) fb_entries;
	LIMINE_PTR(struct limine_file*) executable;
};

extern struct bootloader bootloader;

#define IS_INSIDE_HHDM(_addr) \
	((size_t)(_addr) >= bootloader.hhdm_offset &&	\
	 (size_t)(_addr) <= (bootloader.hhdm_offset + bootloader.mm_total))

int boot_init(void);

#endif
