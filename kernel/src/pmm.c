#include <bootloader.h>
#include <kernel.h>
#include <paging.h>
#include <string.h>
#include <pmm.h>

// Bitmap & helpers
uint32_t* bitmap;
int pmm_ready = 0;
#define SET_BLOCK(_b)	bitmap[(_b) / 32] |= (1 << ((_b) % 32))
#define USET_BLOCK(_b)	bitmap[(_b) / 32] &= ~(1 << ((_b) % 32))
#define TEST_BLOCK(_b)	((bitmap[(_b) / 32] & (1 << ((_b) % 32))) ? 1 : 0)

// Status
size_t usedBlocks = 0;
size_t maxBlocks = 0;

int pmm_init(void) {
	size_t totalmem = (bootloader.memLower + bootloader.memUpper) * 1024;
	maxBlocks = dceil(totalmem, PMM_BS);
	size_t bitmapBytes = dceil(maxBlocks, PMM_BPB);
	usedBlocks = maxBlocks;
	bitmap = NULL;

	// # of pagetabs required to map the bitmap 
	size_t ptcount = dceil(dceil(bitmapBytes, PAGE_SIZE), 1024);

	// Find enough space for the bitmap
	struct boot_memmap* mmap;
	int found = 0;
	for (size_t i = 0; i < bootloader.memmapLen; i++) {
		mmap = &bootloader.memmap[i];

		if (mmap->type == BOOT_MMAP_AVAILABLE &&
		    mmap->size >= maxBlocks / PMM_BPB) {
			found = 1;
			break;
		}
	}
	if (!found) return 1; // Not enough memory

	uint32_t pageFrameStart = mmap->base;
	pag_registerTempFrame(pageFrameStart);
	mmap->base += ptcount * PAGE_SIZE;
	
	uint32_t bitmapStart = KERNEL_HEAP - dceil(bitmapBytes, PAGE_SIZE) * PAGE_SIZE;
	uint32_t bitmapStartPhys = mmap->base;
	bitmap = (uint32_t*)bitmapStart;

	size_t pagecount = dceil(bitmapBytes, PAGE_SIZE);
	for (size_t i = 0; i < pagecount; i++) {
		// We can't unmap this with pag_umapPage(), it's in a tempFrame
		pag_mapPage(bitmapStart + i * PAGE_SIZE, mmap->base + i * PAGE_SIZE, 0);
	}

	// Set all blocks as used
	memset(bitmap, 0xff, maxBlocks / PMM_BPB);

	for (size_t i = 0; i < bootloader.memmapLen; i++) {
		mmap = &bootloader.memmap[i];

		if (mmap->type == BOOT_MMAP_AVAILABLE) pmm_initRegion(mmap->base, mmap->size);
	}

	pmm_deinitRegion(bitmapStartPhys, bitmapBytes);
	pmm_deinitRegion(pageFrameStart, ptcount * PAGE_SIZE);

	debugf(
		"[pmm] Physical memory manager initialized.\n"
		"\tAvailable memory: %d blocks free (%d bytes)\n"
		"\tBitmap at phys(%08x), virt(%08x), %d bytes\n",
		maxBlocks - usedBlocks, (maxBlocks - usedBlocks) * PMM_BS, 
		bitmapStartPhys, bitmapStart, bitmapBytes
	      );

	pmm_ready = 1;
	return 0;
}

int findFreeBlocks(size_t blockCount) {
	if (blockCount == 0) return -1;
	
	uint32_t max = maxBlocks / 32;
	uint32_t freeCount = 0;
	for (size_t i = 0; i < max; i++) {
		// We don't have to check, cause it's all used
		if (bitmap[i] == 0xffffffff) continue;

		for (uint8_t j = 0; j < 32; j++) {
			if (TEST_BLOCK(i * PMM_BPB + j) == 0) freeCount++;
			else freeCount = 0;

			if (freeCount >= blockCount) return i * PMM_BPB + j;
		}
	}

	// We didn't find enough free memory :(
	return -1;
}

void pmm_initRegion(size_t base, size_t len) {
	uint32_t align = base / PMM_BS;
	uint32_t blockCount = len / PMM_BS;

	while (blockCount-- > 0) {
		USET_BLOCK(align);
		align++;
		usedBlocks--;
	}

	SET_BLOCK(0);
}

void pmm_deinitRegion(size_t base, size_t len) {
	uint32_t align = base / PMM_BS;
	uint32_t blockCount = len / PMM_BS;

	while (blockCount-- > 0) {
		SET_BLOCK(align); // We can't increment here, because macros -_(._.)_-
		align++;
		usedBlocks++;
	}

	SET_BLOCK(0);
}

void* pmm_alloc(size_t blockCount) {
	if ((maxBlocks - usedBlocks) < blockCount) return NULL;

	int startBlock = findFreeBlocks(blockCount);
	if (startBlock == -1) return NULL;

	for (size_t i = 0; i < blockCount; i++) {
		SET_BLOCK(startBlock + i);
		usedBlocks++;
	}

	return (void*)(startBlock * PMM_BS);
}

void pmm_free(void* ptr, size_t blockCount) {
	size_t startBlock = (size_t)ptr / PMM_BS;

	for (size_t i = 0; i < blockCount; i++) {
		USET_BLOCK(startBlock + i);
		usedBlocks--;
	}

	SET_BLOCK(0);	// Make sure block 0 is always set
}

