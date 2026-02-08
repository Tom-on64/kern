#ifndef _PAGING_H
#define _PAGING_H

#include <kernel.h>

#define PAGE_FLAG_PRESENT	(1 << 0) // Page is present in the table
#define PAGE_FLAG_WRITE		(1 << 1) // Read-write
#define PAGE_FLAG_USER		(1 << 2) // User-mode (DPL3) access allowed
#define PAGE_FLAG_PWT		(1 << 3) // Page write-thru
#define PAGE_FLAG_NOCACHE	(1 << 4) // Cache disable
#define PAGE_FLAG_ACCESSED	(1 << 5) // Indicates whether page was accessed
#define PAGE_FLAG_DIRTY		(1 << 6) // Page size (valid for PD and PDPT only)
#define PAGE_FLAG_PAT		(1 << 7) // Page Attribute Table (valid for PT only)
#define PAGE_FLAG_GLOBAL	(1 << 8) // Indicates the page is globally cached
#define PAGE_FLAG_SHARED	(1 << 9) // Userland page is shared
// Region caching according to the Limine protocol
#define PAGE_FLAG_CACHE	(PAGE_FLAG_PWT | PAGE_FLAG_PAT)

#define PAGE_SIZE	0x1000
#define PAGE_SIZE_LARGE	0x200000
#define PAGE_SIZE_HUGE	0x40000000

#define PHYS_ADDR(_x) ((_x) & ~0xFFF)

int	pag_init(void);
void	pag_map(size_t vaddr, size_t paddr, size_t flags);
size_t	pag_umap(size_t vaddr);
size_t	pag_virt_to_phys(size_t vaddr);
size_t*	pag_get_pagedir();
size_t*	pag_get_task_pagedir(void* task);
void	pag_set_pagedir(size_t* pagedir);
size_t*	pag_alloc_pagedir();
void	pag_free_pagedir(size_t* pagedir);
void	pag_invalidate(size_t vaddr);

#endif
