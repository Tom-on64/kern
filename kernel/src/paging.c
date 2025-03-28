#include <kernel.h>
#include <string.h>
#include <paging.h>

// Invalidates the pagedir entry in the CPUs TLB cache
static inline void invalidate(uint32_t vaddr) {
	__asm__ volatile ("invlpg %0" : : "m"(vaddr));
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
	init_pagedir[1023] = ((uint32_t)init_pagedir - 0xC0000000) |
		PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;
	invalidate(0xFFFFF000);

	return 0;
}

