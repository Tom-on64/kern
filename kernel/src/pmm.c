#include "bitmap.h"
#include <bootloader.h>
#include <kernel.h>
#include <paging.h>
#include <string.h>
#include <pmm.h>

// Bitmap
bitmap_t pmm_bitmap;

int pmm_init(void) {
	pmm_bitmap.blocks = dceil(bootloader.mm_total, BLOCK_SIZE);
	pmm_bitmap.bytes = dceil(pmm_bitmap.blocks, 8);

	// # of pagetabs required to map the bitmap 
	size_t ptcount = dceil(dceil(pmm_bitmap.bytes, PAGE_SIZE), 1024);

	// Find enough space for the bitmap
	struct limine_memmap_entry* mm = NULL;
	for (size_t i = 0; i < bootloader.mm_entry_count; i++) {
		struct limine_memmap_entry* entry = bootloader.mm_entries[i];

		if (entry->type != LIMINE_MEMMAP_USABLE || entry->length < pmm_bitmap.bytes) continue;
		mm = entry;
	}

	if (!mm) {
		debugf("[pmm] Not enough memory! Needs %d bytes.\n", pmm_bitmap.bytes);
		return 1;
	}

	size_t bitmap_start_phys = mm->base;
	pmm_bitmap.map = (void*)(bitmap_start_phys + bootloader.hhdm_offset);

	// Set all blocks as used
	bmap_set_all(&pmm_bitmap, 1);

	debugf("[pmm] Memory map:\n"); // Use the loop to also print the memmap
	for (size_t i = 0; i < bootloader.mm_entry_count; i++) {
		struct limine_memmap_entry* entry = bootloader.mm_entries[i];
		if (entry->length == 0) continue; // Ignore dummy entries

		if (entry->type == LIMINE_MEMMAP_USABLE) {
			bmap_set_area(&pmm_bitmap, entry->base, entry->length, 0);
		}

		// Print entry
		debugf("\tRegion %2d: 0x%08x - 0x%08x (%d bytes) %s\n",
			i + 1, entry->base, entry->base + entry->length - 1, entry->length, 
			(entry->type == LIMINE_MEMMAP_USABLE) ? "AVAILABLE" : "RESERVED");
	}
	bmap_set_area(&pmm_bitmap, bitmap_start_phys, pmm_bitmap.bytes, 1);
	pmm_bitmap.used = 0;

	// Beatiful print statement :)
	debugf(
		"[pmm] Physical memory manager initialized.\n"
		"\tAvailable memory: %d blocks free (%d bytes)\n"
		"\tBitmap at 0x%08x in physical memory\n"
		"\tBitmap is %d bytes contained in %d pages\n",
		pmm_bitmap.blocks - pmm_bitmap.used,
		(pmm_bitmap.blocks - pmm_bitmap.used) * BLOCK_SIZE,
		bitmap_start_phys, pmm_bitmap.bytes
	      );

	pmm_bitmap.ready = 1;
	return 0;
}

size_t pmm_alloc(void) {
	if (pmm_bitmap.blocks - pmm_bitmap.used == 0) return 0;

	size_t base = bmap_get_area(&pmm_bitmap, 1, 0);
	if (base == BMAP_NOT_FOUND) return 0;
	
	bmap_set(&pmm_bitmap, base, 1);

	return (size_t)bmap_ptr(&pmm_bitmap, base);
}

void pmm_free(size_t page) {
	size_t base = page / BLOCK_SIZE;
	bmap_set_area(&pmm_bitmap, base, 1, 0);
	bmap_set(&pmm_bitmap, 0, 1);
}

