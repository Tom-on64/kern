#include <bootloader.h>
#include <kernel.h>
#include <limine.h>
#include <string.h>
#include <serial.h>
#include <pmm.h>

#include <paging.h>

// VIRTUAL address of the kernels PML4
uint64_t* pag_kernel_pml4 = NULL;

int pag_init(void) {
	pag_kernel_pml4 = TO_VIRT(pmm_alloc());
	if (pag_kernel_pml4 == NULL) {
		debugf("[pag] Failed to allocate kernel PML4. Not enough memory.\n");
		return 1;
	}
	memset(pag_kernel_pml4, 0, PAGE_SIZE);

	pag_map_region(
		pag_kernel_pml4, 
		(void*)bootloader.kernel_virt_base, 
		(void*)bootloader.kernel_phys_base, 
		dceil(bootloader.executable->size, PAGE_SIZE), 
		PTE_FLAG_WRITE | PTE_FLAG_PRESENT
	);

	for (size_t i = 0; i < bootloader.mm_entry_count; i++) {
		struct limine_memmap_entry* entry = bootloader.mm_entries[i];
		
		uint64_t flags = 0;
		switch (entry->type) {
		case LIMINE_MEMMAP_USABLE: flags = PTE_FLAG_WRITE; break;
		case LIMINE_MEMMAP_RESERVED: continue;
		case LIMINE_MEMMAP_ACPI_RECLAIMABLE: flags = PTE_FLAG_WRITE; break;   
		case LIMINE_MEMMAP_ACPI_NVS: break;
		case LIMINE_MEMMAP_BAD_MEMORY: continue;
		case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE: flags = PTE_FLAG_WRITE; break;
		case LIMINE_MEMMAP_EXECUTABLE_AND_MODULES: flags = PTE_FLAG_WRITE; break;
		case LIMINE_MEMMAP_FRAMEBUFFER: flags = PTE_FLAG_WRITE | PTE_FLAG_PWT; break;
		case LIMINE_MEMMAP_RESERVED_MAPPED: flags = 0; break;
		default: 
			debugf(
				"[pag] %p - %p: Unknown memmap type %d. Will remain unmapped.\n",
				entry->base, entry->base + entry->length, entry->type
			      );
			continue;
		}

		pag_map_region(
			pag_kernel_pml4,
			(void*)(entry->base + bootloader.hhdm_offset), (void*)entry->base,
			dceil(entry->length, PAGE_SIZE),
			flags | PTE_FLAG_PRESENT
		);
	}

	debugf("[pag] Switching to kernel PML4 at %p.\n", pag_kernel_pml4);
	pag_set_pml4(pag_kernel_pml4);

	debugf("[pag] Paging initialized.\n");
	return 0;
}

uint64_t* pag_get_pte(uint64_t* pml4, void* vaddr) {
	uintptr_t v = (uintptr_t)vaddr;
	if (v % PAGE_SIZE != 0) {
		debugf("[pag] Could not get PTE from non-aligned virtual address %p.\n", vaddr);
		return NULL;
	}

	uint64_t* pdpt = pag_get_next_tab(pml4,	PML4_IDX(v));
	if (pdpt == NULL) {
		debugf("[pag] Could not get PTE. %p was not found in PML4.\n");
		return NULL;
	}
	uint64_t* pd = pag_get_next_tab(pdpt, PDPT_IDX(v));
	if (pd == NULL) {
		debugf("[pag] Could not get PTE. %p was not found in PDPT.\n");
		return NULL;
	}
	uint64_t* pt = pag_get_next_tab(pd, PD_IDX(v));
	if (pt == NULL) {
		debugf("[pag] Could not get PTE. %p was not found in PD.\n");
		return NULL;
	}

	return &pt[PT_IDX(v)];
}

uint64_t* pag_new_pte(uint64_t* pml4, void* vaddr) {
	uintptr_t v = (uintptr_t)vaddr;
	if (v % PAGE_SIZE != 0) {
		debugf("[pag] Could not create PTE for non-aligned virtual address %p.\n", vaddr);
		return NULL;
	}

	uint64_t* pdpt = pag_new_next_tab(pml4,	PML4_IDX(v));
	if (pdpt == NULL) {
		debugf("[pag] Failed to create PDPT. Not enough memory.\n");
		return NULL;
	}
	uint64_t* pd = pag_new_next_tab(pdpt, PDPT_IDX(v));
	if (pd == NULL) {
		debugf("[pag] Failed to create PD. Not enough memory.\n");
		if (pdpt != NULL) pmm_free(pdpt);
		return NULL;
	}
	uint64_t* pt = pag_new_next_tab(pd, PD_IDX(v));
	if (pt == NULL) {
		debugf("[pag] Failed to create PT. Not enough memory.\n");
		if (pd != NULL) pmm_free(pd);
		if (pdpt != NULL) pmm_free(pdpt);
		return NULL;
	}

	return &pt[PT_IDX(v)];
}

void pag_map(uint64_t* pml4, void* vaddr, void* paddr, uint64_t flags) {
	uintptr_t p = (uintptr_t)paddr;
	if (p % PAGE_SIZE != 0) {
		debugf("[pag] Could not map %p to %p. Physical address is not aligned.\n", paddr, vaddr);
		return;
	}
	
	uint64_t* page = pag_new_pte(pml4, vaddr);
	*page = PTE_ADDR(p) | flags | PTE_FLAG_PRESENT;
	pag_invalidate(vaddr);
}

void pag_map_region(uint64_t* pml4, void* vaddr, void* paddr, size_t count, uint64_t flags) {
	for (size_t i = 0; i < count; i++) {
		pag_map(pml4, vaddr + i * PAGE_SIZE, paddr + i * PAGE_SIZE, flags);
	}
}

// TODO: This is a pmm memory leak. If all pages in a PT are unmapped, remove
// that PT from it's PD. This applies to PDs & PDPTs too.
void pag_unmap(uint64_t* pml4, void* vaddr) {
	uint64_t* pte = pag_get_pte(pml4, vaddr);
	*pte = 0;
	pag_invalidate(vaddr);
}

void pag_unmap_region(uint64_t* pml4, void* vaddr, size_t count) {
	for (size_t i = 0; i < count; i++) {
		pag_unmap(pml4, vaddr + i * PAGE_SIZE);
	}
}

void pag_protect(uint64_t* pml4, void* vaddr, uint64_t flags) {
	uint64_t* pte = pag_get_pte(pml4, vaddr);
	*pte = PTE_ADDR(*pte) | flags | PTE_FLAG_PRESENT;
}

void* pag_get_phys(uint64_t* pml4, void* vaddr) {
	uint64_t* pte = pag_get_pte(pml4, vaddr);
	return (void*)PTE_ADDR(*pte);
}

void pag_set_pml4(uint64_t* pml4) {
	__asm__ volatile("mov %0, %%cr3" :: "r" (TO_PHYS(pml4)) : "memory");
}

uint64_t* pag_get_pml4(void) {
	uint64_t* pml4;
	__asm__ volatile("movq %%cr3, %0" : "=r"(pml4) :: "memory");
	return TO_VIRT(pml4);
}

uint64_t* pag_get_next_tab(uint64_t* tab, offset_t index) {
	if (index >= 512 || tab == NULL) return NULL;
	else if (tab[index] & PTE_FLAG_PRESENT) return TO_VIRT(tab[index] & PTE_ADDR_MASK);
	else return NULL;
}

uint64_t* pag_new_next_tab(uint64_t* tab, offset_t index) {
	if (tab == NULL) return NULL;
	uint64_t* vaddr = pag_get_next_tab(tab, index);
	if (vaddr != NULL) return vaddr;

	void* paddr = pmm_alloc();
	if (!paddr) {
		debugf("[pag] Failed to allocate page for new table.\n");
		return NULL;
	}

	vaddr = TO_VIRT(paddr);
	memset(vaddr, 0, PAGE_SIZE);

	tab[index] = (uintptr_t)paddr | PTE_FLAG_PRESENT | PTE_FLAG_WRITE | PTE_FLAG_USER;
	return vaddr;
}

