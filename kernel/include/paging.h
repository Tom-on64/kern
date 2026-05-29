#ifndef _PAGING_H
#define _PAGING_H

#include <bootloader.h>
#include <kernel.h>

#define PTE_FLAG_PRESENT	(1UL << 0) // Page is present in the table
#define PTE_FLAG_WRITE		(1UL << 1) // Read-write
#define PTE_FLAG_USER		(1UL << 2) // User-mode (DPL3) access allowed
#define PTE_FLAG_PWT		(1UL << 3) // Page write-thru
#define PTE_FLAG_NOCACHE	(1UL << 4) // Cache disable
#define PTE_FLAG_FETCH		(1UL << 4) // Fetch
#define PTE_FLAG_ACCESSED	(1UL << 5) // Indicates whether page was read/written
#define PTE_FLAG_DIRTY		(1UL << 6) // Indicates whether page was written to (valid for PT only)
#define PTE_FLAG_PAT		(1UL << 7) // Page Attribute Table (valid for PT only)
#define PTE_FLAG_PAGESIZE	(1UL << 7) // Page size (valid for PD and PDPT only)
#define PTE_FLAG_RSVD		(1UL << 7) // No idea (valid for PML4 and PML5 only)
#define PTE_FLAG_GLOBAL		(1UL << 8) // Global caching or sum (valid for PT only)
#define PTE_FLAG_NOEXEC		(1UL << 63) // No execute (the XD bit)
// Region caching according to the Limine protocol
#define PTE_FLAG_CACHE		(PTE_FLAG_PWT | PTE_FLAG_PAT)

#define PTE_ADDR_MASK	0x000FFFFFFFFFF000UL
#define PTE_ADDR(_pte)	((_pte) & PTE_ADDR_MASK)

#define PML4_IDX(_ptr)	(((_ptr) >> 39) & 0x1FFUL)
#define PDPT_IDX(_ptr)	(((_ptr) >> 30) & 0x1FFUL)
#define PD_IDX(_ptr)	(((_ptr) >> 21) & 0x1FFUL)
#define PT_IDX(_ptr)	(((_ptr) >> 12) & 0x1FFUL)

#define TO_VIRT(_ptr) 	((void*)((uintptr_t)(_ptr) + bootloader.hhdm_offset))
#define TO_PHYS(_ptr) 	((void*)((uintptr_t)(_ptr) - bootloader.hhdm_offset))

#define PAGE_SIZE 4096
#define PAGE_SIZE_LARGE	0x200000
#define PAGE_SIZE_HUGE	0x40000000

// WARN: This will only invalidate on one core
static inline void pag_invalidate(void* vaddr) { __asm__ volatile("invlpg (%0)" :: "r" (vaddr) : "memory"); }
static inline bool pag_is_user_addr(void* vaddr) { return (uintptr_t)vaddr < 0x0000800000000000UL; }

// NOTE: All uint64_t* are VIRTUAL addresses
int	  pag_init(void);
uint64_t* pag_get_pte(uint64_t* pml4, void* vaddr);
int	  pag_map(uint64_t* pml4, void* vaddr, void* paddr, uint64_t flags);
int	  pag_map_region(uint64_t* pml4, void* vaddr, void* paddr, size_t count, uint64_t flags);
int	  pag_unmap(uint64_t* pml4, void* vaddr);
int	  pag_unmap_region(uint64_t* pml4, void* vaddr, size_t count);
int	  pag_protect(uint64_t* pml4, void* vaddr, uint64_t flags);
void*	  pag_get_phys(uint64_t* pml4, void* vaddr);
void	  pag_set_pml4(uint64_t* pml4);
uint64_t* pag_get_pml4(void);
uint64_t* pag_get_next_tab(uint64_t* tab, offset_t index);
uint64_t* pag_new_next_tab(uint64_t* tab, offset_t index);

#endif
