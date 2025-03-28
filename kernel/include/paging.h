#ifndef _PAGING_H
#define _PAGING_H

#include <kernel.h>

#define PAGE_SIZE	4096

#define PHYS_ADDR(_a)	((_a) & ~0xfff)

#define PAGE_FLAG_PRESENT	(1 << 0)
#define PAGE_FLAG_WRITE		(1 << 1)
#define PAGE_FLAG_USER		(1 << 2)
#define PAGE_FLAG_PWT		(1 << 3)
#define PAGE_FLAG_NOCACHE	(1 << 4)
#define PAGE_FLAG_ACCESSED	(1 << 5)
#define PAGE_FLAG_DIRTY		(1 << 6)
#define PAGE_FLAG_4MB		(1 << 7)
#define PAGE_FLAG_GLOBAL	(1 << 8)

// Defined in entry.s for initial paging
extern uint32_t init_pagedir[1024];

int pag_init(void);

#endif
