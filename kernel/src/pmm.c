#include <bootloader.h>
#include <kernel.h>
#include <string.h>
#include <pmm.h>

// Bitmap & helpers
uint32_t* bitmap = NULL;
#define SET_BLOCK(_b)	bitmap[(_b) / 32] |= (1 << ((_b) % 32))
#define USET_BLOCK(_b)	bitmap[(_b) / 32] &= ~(1 << ((_b) % 32))
#define TEST_BLOCK(_b)	((bitmap[(_b) % 32] & (1 << ((_b) % 32))) ? 1 : 0)

// Status
size_t usedBlocks = 0;
size_t maxBlocks = 0;

int pmm_init(void) {
	size_t totalmem = (bootloader.memLower + bootloader.memUpper) * 1024;
	maxBlocks = totalmem / PMM_BS;
	usedBlocks = maxBlocks;

	// TODO:
	bitmap = (uint32_t*)NULL;
	memset(bitmap, 0, maxBlocks / PMM_BPB);

	return 0;
}

int findFreeBlocks(size_t blockCount) {
	if (blockCount == 0) return -1;
	
	uint32_t max = maxBlocks / 32;
	uint32_t freeCount = 0;
	for (size_t i = 0; i < max; i++) {
		// We don't have to check, cause it's all reserved
		if (bitmap[i] == 0xffffffff) continue;

		for (uint8_t j = 0; j < 32; j++) {
			if (TEST_BLOCK(i * PMM_BPB + j)) freeCount++;
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
		align++;
		SET_BLOCK(align); // We can't increment here, because macros -_(._.)_-
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

