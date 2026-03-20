#ifndef _PAGING_H
#define _PAGING_H

#include <kernel.h>

#define PTE_FLAG_PRESENT	(1ULL << 0) // Page is present in the table
#define PTE_FLAG_WRITE		(1ULL << 1) // Read-write
#define PTE_FLAG_USER		(1ULL << 2) // User-mode (DPL3) access allowed
#define PTE_FLAG_PWT		(1ULL << 3) // Page write-thru
#define PTE_FLAG_NOCACHE	(1ULL << 4) // Cache disable
#define PTE_FLAG_ACCESSED	(1ULL << 5) // Indicates whether page was accessed
#define PTE_FLAG_DIRTY		(1ULL << 6) // Page size (valid for PD and PDPT only)
#define PTE_FLAG_PAT		(1ULL << 7) // Page Attribute Table (valid for PT only)
#define PTE_FLAG_GLOBAL		(1ULL << 8) // Indicates the page is globally cached
#define PTE_FLAG_SHARED		(1ULL << 9) // Userland page is shared
#define PTE_FLAG_NOEXEC		(1ULL << 63) // No execute
// Region caching according to the Limine protocol
#define PTE_FLAG_CACHE		(PTE_FLAG_PWT | PTE_FLAG_PAT)

#define PTE_ADDR_MASK	0x000FFFFFFFFFFFFFF000ULL
#define PTE_ADDR(_pte)	((_pte) & PAGE_ADDR_MASK)

#define PAGE_SIZE	0x1000
#define PAGE_SIZE_LARGE	0x200000
#define PAGE_SIZE_HUGE	0x40000000

// From the Linux kernel source
// WARN: This will only invalidate on one core
static inline void pag_invalidate(size_t vaddr) { __asm__ volatile("invlpg (%0)" :: "r" (vaddr) : "memory"); }

int	pag_init(void);
void	pag_map(uint64_t* pml4, void* vaddr, void* paddr, uint64_t flags);
void	pag_unmap(uint64_t* pml4, void* vaddr);
void	pag_switch(uint64_t* pml4);
uint64_t* pag_new_pml(void);

#endif
