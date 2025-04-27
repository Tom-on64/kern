#ifndef _PAGING_H
#define _PAGING_H

#include <kernel.h>

#define PHYS_ADDR(_a)	((_a) & ~0xFFF)

// The kernels pagedir
#define PAGEDIR		((uint32_t*)(0xFFFFF000))
#define PAGETAB(_i)	((uint32_t*)(0xFFC00000 + ((_i) << 12)))

#define PAGE_SIZE	4096

#define PAGE_FLAG_PRESENT	(1 << 0)
#define PAGE_FLAG_WRITE		(1 << 1)
#define PAGE_FLAG_USER		(1 << 2)
#define PAGE_FLAG_PWT		(1 << 3)
#define PAGE_FLAG_NOCACHE	(1 << 4)
#define PAGE_FLAG_ACCESSED	(1 << 5)
#define PAGE_FLAG_DIRTY		(1 << 6) // AVL in PDE where PS = 0
#define PAGE_FLAG_PS		(1 << 7) // If PS = 1 then directly maps a 4MB page
#define PAGE_FLAG_GLOBAL	(1 << 8)
// bits 9-11 are AVL - OS is free to use these as its own flags
#define PAGE_FLAG_OWNER		(1 << 9)

extern uint32_t	init_pagedir[1024]; // Defined in entry.s for initial paging
extern size_t	pag_page_count;

int pag_init(void);
void pag_register_temp_frame(uint32_t target);
uint32_t pag_temp_frame(void);
void pag_map(uint32_t vaddr, uint32_t paddr, uint32_t flags);
uint32_t pag_umap(uint32_t vaddr);
void* pag_virt_to_phys(uint32_t vaddr);
uint32_t* pag_get_pagedir();
void pag_set_pagedir(uint32_t* pagedir);
void pag_sync_pagedir();
uint32_t* pag_alloc_pagedir();
void pag_free_pagedir(uint32_t* pagedir);

#endif
