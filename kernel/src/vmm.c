#include <bitmap.h>
#include <kernel.h>
#include <paging.h>
#include <pmm.h>

#include <vmm.h>

#include <string.h>

bitmap_t vmm_bitmap;

int vmm_init(void) {
	vmm_bitmap.ready = 0;
	vmm_bitmap.membase = KERNEL_HEAP_BASE;
	vmm_bitmap.blocks = dceil(KERNEL_HEAP_SIZE, BLOCK_SIZE);
	vmm_bitmap.bytes = dceil(vmm_bitmap.blocks, 8);
	uint32_t bitmaploc = 0xC6000000;

	size_t pagecount = dceil(vmm_bitmap.bytes, PAGE_SIZE);
	for (size_t i = 0; i < pagecount; i++) {
		void* fr = pmm_allocPage();
		if (fr == NULL) return 1;
		pag_mapPage(bitmaploc + i * PAGE_SIZE, (uint32_t)fr, 0);
	}

	// Set all pages as
	vmm_bitmap.map = (void*)bitmaploc;
	bmap_setall(&vmm_bitmap, 0);

	bmap_set(&vmm_bitmap, 0, 1);
	bmap_set(&vmm_bitmap, 8, 1);

	debugf(
		"[vmm] Virtual memory manager initialized.\n"
		"\tBitmap manages %d blocks\n"
		"\tBitmap is at 0x%08x in virtual memory\n"
		"\tBitmap is %d bytes over %d pages\n",
		vmm_bitmap.blocks, vmm_bitmap.map,
		vmm_bitmap.bytes, pagecount
	      );

	vmm_bitmap.ready = 1;
	return 0;
}

void* vmm_alloc(size_t pages) {
	size_t idx = bmap_findarea(&vmm_bitmap, pages, 0);
	if (idx == BMAP_NOT_FOUND) return NULL;
	void* ptr = bmap_ptr(&vmm_bitmap, idx);

	bmap_setarea(&vmm_bitmap, idx * BLOCK_SIZE, pages * PAGE_SIZE, 1);

	for (size_t i = 0; i < pages; i++) {
		void* fr = pmm_allocPage();
		if (fr == NULL) return NULL;
		pag_mapPage((uint32_t)ptr + i * PAGE_SIZE, (uint32_t)fr, 0);
	}

	return ptr;
}

void vmm_free(void* ptr, size_t pages) {
	for (size_t i = 0; i < pages; i++) {
		uint32_t vaddr = (uint32_t)ptr + i * PAGE_SIZE;
		uint32_t paddr = (uint32_t)pag_virtToPhys((uint32_t)ptr);
		
		pmm_freePage((void*)paddr);
		pag_umapPage(vaddr);
	}

	size_t base = ((size_t)ptr - KERNEL_BASE) / BLOCK_SIZE;
	bmap_setarea(&vmm_bitmap, base, pages, 0);
}

