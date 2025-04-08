#include <bootloader.h>
#include <kernel.h>
#include <paging.h>
#include <string.h>
#include <pmm.h>

// Bitmap
bitmap_t pmm_bitmap;

int pmm_init(void) {
	// Full 32-bit address space
	pmm_bitmap.membase = 0x00000000;
	pmm_bitmap.blocks = 0x100000;	// 0xFFFFFFFF / BLOCK_SIZE
	pmm_bitmap.bytes = 0x20000;	// blocks / 8
	pmm_bitmap.map = NULL;

	// # of pagetabs required to map the bitmap 
	size_t ptcount = dceil(dceil(pmm_bitmap.bytes, PAGE_SIZE), 1024);

	// Find enough space for the bitmap
	struct boot_memmap* mmap;
	int found = 0;
	for (size_t i = 0; i < bootloader.memmapLen; i++) {
		mmap = &bootloader.memmap[i];

		if (mmap->type == BOOT_MMAP_AVAILABLE && mmap->size >= pmm_bitmap.bytes) {
			found = 1;
			break;
		}
	}
	if (!found) return 1; // Not enough memory

	uint32_t pageFrameStart = mmap->base;
	pag_registerTempFrame(pageFrameStart);
	mmap->base += ptcount * PAGE_SIZE;
	
	uint32_t bitmapStart = KERNEL_HEAP_BASE - dceil(pmm_bitmap.bytes, PAGE_SIZE) * PAGE_SIZE;
	uint32_t bitmapStartPhys = mmap->base;
	pmm_bitmap.map = (void*)bitmapStart;

	size_t pagecount = dceil(pmm_bitmap.bytes, PAGE_SIZE);
	for (size_t i = 0; i < pagecount; i++) {
		// NOTE: We can't unmap this with pag_umapPage(), it's in a tempFrame
		pag_mapPage(bitmapStart + i * PAGE_SIZE, mmap->base + i * PAGE_SIZE, 0);
	}

	// Set all blocks as used
	bmap_setall(&pmm_bitmap, 1);

	debugf("[pmm] Memory map:\n"); // Use the loop to also print the memmap

	for (size_t i = 0; i < bootloader.memmapLen; i++) {
		mmap = &bootloader.memmap[i];
		if (mmap->size == 0) continue; // Ignore dummy entries

		if (mmap->type == BOOT_MMAP_AVAILABLE) {
			bmap_setarea(&pmm_bitmap, mmap->base, mmap->size, 0);
		}

		// Print entry
		debugf("\tRegion %2d: 0x%08x - 0x%08x (%d bytes) %s\n",
			i + 1, mmap->base, mmap->base + mmap->size - 1, mmap->size, 
			(mmap->type == BOOT_MMAP_AVAILABLE) ? "AVAILABLE" : "RESERVED");
	}

	bmap_setarea(&pmm_bitmap, bitmapStartPhys, pmm_bitmap.bytes, 1);
	bmap_setarea(&pmm_bitmap, pageFrameStart, ptcount * PAGE_SIZE, 1);
	bmap_set(&pmm_bitmap, 0, 1); // Reserve block 0 - NULL page

	// Beatifull print statement :)
	debugf(
		"[pmm] Physical memory manager initialized.\n"
		"\tAvailable memory: %d blocks free (%d bytes)\n"
		"\tBitmap at 0x%08x mapped to 0x%08x\n"
		"\tBitmap is %d bytes contained in %d pages\n",
		pmm_bitmap.blocks - pmm_bitmap.used,
		(pmm_bitmap.blocks - pmm_bitmap.used) * BLOCK_SIZE,
		bitmapStartPhys, bitmapStart, pmm_bitmap.bytes, pagecount
	      );

	pmm_bitmap.ready = 1;
	return 0;
}

void* pmm_allocPage(void) {
	if (pmm_bitmap.blocks - pmm_bitmap.used == 0) return NULL;

	size_t base = bmap_findarea(&pmm_bitmap, 1, 0);
	if (base == BMAP_NOT_FOUND) return NULL;
	
	bmap_set(&pmm_bitmap, base, 1);

	return bmap_ptr(&pmm_bitmap, base);
}

void pmm_freePage(void* page) {
	size_t base = (size_t)page / BLOCK_SIZE;
	bmap_setarea(&pmm_bitmap, base, 1, 0);
	bmap_set(&pmm_bitmap, 0, 1);
}

