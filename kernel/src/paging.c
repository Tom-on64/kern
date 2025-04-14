#include <kernel.h>
#include <string.h>
#include <pmm.h>

#include <paging.h>

#define PAGE_DIR_COUNT	256
static uint32_t	pagedirs[PAGE_DIR_COUNT][1024] __align(4096);
static uint8_t	pagedirUsed[PAGE_DIR_COUNT];

// TODO: Should this be initialized? If so, to what?
size_t pag_pageCount;

// Invalidates the pagedir entry in the CPUs TLB cache
static inline void invalidate(uint32_t vaddr) {
	__asm__ volatile ("invlpg %0" : : "m"(vaddr));
}

// Temp paging thing
uint32_t tempFrameStart;
void pag_registerTempFrame(uint32_t target) { tempFrameStart = target; }
uint32_t pag_tempFrame(void) {
	uint32_t fin = tempFrameStart;
	tempFrameStart += PAGE_SIZE;
	return fin;
}

int pag_init(void) {
	// Unmap first MB
	init_pagedir[0] = 0;
	invalidate(0);

	/*
	 * Recursive table mapping
	 * We map the last pagetab to the pagedir itself, so it acts
	 * as a pagetab even though it's a pagedir. This maps it to
	 * the virtual address 0xFFFFF000.
	 */
	init_pagedir[1023] = ((uint32_t)init_pagedir - KERNEL_BASE) |
		PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;
	invalidate(0xFFFFF000);

	memset(pagedirs, 0, PAGE_SIZE * PAGE_DIR_COUNT);
	memset(pagedirUsed, 0, PAGE_DIR_COUNT);

	debugf("[pag] Paging initalized.\n");

	return 0;
}

void pag_mapPage(uint32_t vaddr, uint32_t paddr, uint32_t flags) {
	// Save the pagedir we were in
	uint32_t* prevpd = NULL;

	// Switch to the init pagedir if in kernel memory
	if (vaddr >= KERNEL_BASE) {
		prevpd = pag_getPagedir();
		if (prevpd != init_pagedir) pag_setPagedir(init_pagedir);
	}

	// Extract indecies
	size_t pdi = vaddr >> 22;
	size_t pti = vaddr >> 12 & 0x3FF;

	uint32_t* pagedir = PAGEDIR;
	uint32_t* pagetab = PAGETAB(pdi);

	// Allocate a pagetab if it didn't exist
	if (!(pagedir[pdi] & PAGE_FLAG_PRESENT)) {
		uint32_t tabaddr = pmm_bitmap.ready ? (uint32_t)pmm_allocPage() : pag_tempFrame();

		pagedir[pdi] = tabaddr |
			PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE | PAGE_FLAG_OWNER | flags;
		invalidate(vaddr);


		for (size_t i = 0; i < 1024; i++) {
			pagetab[i] = 0;
		}

	}

	// The actuall mapping
	pagetab[pti] = paddr | PAGE_FLAG_PRESENT | flags;
	pag_pageCount++;
	invalidate(vaddr);

	// Do we need to return to the original pagedir?
	if (prevpd != NULL) {
		pag_syncPagedir(); // Sync it to other pagedirs
		if (prevpd != init_pagedir) pag_setPagedir(prevpd);
	}
}

uint32_t pag_umapPage(uint32_t vaddr) {
	// Same saving as in pag_mapPage()
	uint32_t* prevpd = NULL;

	if (vaddr >= KERNEL_BASE) {
		prevpd = pag_getPagedir();
		if (prevpd != init_pagedir) pag_setPagedir(init_pagedir);
	}

	// Extract indecies
	size_t pdi = vaddr >> 22;
	size_t pti = vaddr >> 12 & 0x3FF;

	uint32_t* pagedir = PAGEDIR;
	uint32_t* pagetab = PAGETAB(pdi);

	uint32_t pte = pagetab[pti];
	pagetab[pti] = 0;
	pag_pageCount--;

	// Look if we can free the pagetab
	int remove = 1;
	for (size_t i = 0; i < 1024; i++) {
		if (!(pagetab[i] & PAGE_FLAG_PRESENT)) continue;
		remove = 0;
		break;
	}

	// Free the pagetab if possible
	uint32_t pde = pagedir[pdi];
	if (remove && pde & PAGE_FLAG_OWNER) {
		uint32_t tabaddr = PHYS_ADDR(pde);
		pmm_freePage((void*)tabaddr);
		pagedir[pdi] = 0;
	}

	invalidate(vaddr);

	// Free the page frame
	if (pte & PAGE_FLAG_OWNER) pmm_freePage((void*)PHYS_ADDR(pte));

	if (prevpd != NULL) {
		pag_syncPagedir(); // Sync it to other pagedirs
		if (prevpd != init_pagedir) pag_setPagedir(prevpd);
	}

	return pte;
}

void* pag_virtToPhys(uint32_t vaddr) {
	uint32_t pdi = vaddr >> 22;
	uint32_t pti = vaddr >> 12 & 0x3FF;

	uint32_t* pagetab = PAGETAB(pdi);

	return (void*)((pagetab[pti] & ~0xFFF) + ((uint32_t)vaddr & 0xFFF));
}

uint32_t* pag_getPagedir() {
	uint32_t pagedir;
	__asm__ volatile ("mov %%cr3, %0" : "=r"(pagedir));
	pagedir += KERNEL_BASE;
	return (uint32_t*)pagedir;
}

void pag_setPagedir(uint32_t* pagedir) {
	pagedir = (uint32_t*)((uint32_t)pagedir - KERNEL_BASE);
	__asm__ volatile (
		"mov %0, %%eax\n"
		"mov %%eax, %%cr3"
		: : "m"(pagedir)
	);
}

void pag_syncPagedir() {
	for (size_t i = 0; i < PAGE_DIR_COUNT; i++) {
		if (!pagedirUsed[i]) continue;

		uint32_t* pagedir = pagedirs[i];

		// Sync ONLY the kernel pages so 768 has base at 0xC0000000
		// Also don't sync the recursive mapping in the last page
		for (size_t i = 768; i < 1023; i++) {
			pagedir[i] = init_pagedir[i] & ~PAGE_FLAG_OWNER;
		}
	}
}

uint32_t* pag_allocPagedir() {
	// Find the first unused page
	for (size_t i = 0; i < PAGE_DIR_COUNT; i++) {
		if (pagedirUsed[i]) continue;
		pagedirUsed[i] = 1;

		uint32_t* pagedir = pagedirs[i];
		memset(pagedir, 0, PAGE_SIZE);

		// First 768 entries are user pagetabs (3:1 memory split)
		for (size_t i = 0; i < 768; i++) pagedir[i] = 0;

		// Next 255 are kernel space
		for (size_t i = 768; i < 1023; i++) pagedir[i] = init_pagedir[i] & ~PAGE_FLAG_OWNER;

		// Last page is the recursive table mapping
		pagedir[1023] = ((uint32_t)pagedir - KERNEL_BASE) |
			PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;

		return pagedir;
	}

	// No pages left - out of memory
	return NULL;
}

void pag_freePagedir(uint32_t* pagedir) {
	uint32_t* prevpd = pag_getPagedir();
	pag_setPagedir(pagedir);

	uint32_t pdi = (uint32_t)pagedir - (uint32_t)pagedirs;
	pdi /= PAGE_SIZE;

	// Free all allocated page frames & pagetabs
	uint32_t* pd = PAGEDIR;
	for (int i = 0; i < 768; i++) {
		uint32_t pde = pd[i];
		if (pde == 0) continue;

		uint32_t* pagetab = PAGETAB(i);
		for (size_t j = 0; j < 1024; j++) {
			uint32_t pte = pagetab[j];

			if (pte & PAGE_FLAG_OWNER) pmm_freePage((void*)PHYS_ADDR(pte));
		}
		memset(pagetab, 0, PAGE_SIZE);

		if (pde & PAGE_FLAG_OWNER) pmm_freePage((void*)PHYS_ADDR(pde));
		pd[i] = 0;
	}

	pagedirUsed[pdi] = 0;
	pag_setPagedir(prevpd);
}

