#include "serial.h"
#include <bootloader.h>
#include <kernel.h>
#include <string.h>
#include <pmm.h>

#include <paging.h>

size_t* pag_globalpd = NULL;

void pag_invalidate(size_t vaddr) { __asm__ volatile ("invlpg %0" ::"m"(vaddr)); }

int pag_init() {
	size_t pd_phys = 0;
	__asm__ volatile ("movq %%cr3, %0" : "=r"(pd_phys));
	if (!pd_phys) return 1;

	size_t pd_virt = pd_phys + bootloader.hhdm_offset;
	pag_globalpd = (size_t*)pd_virt;

	debugf("[pag] Paging initialized.");

	return 0;
}

void pag_map(size_t vaddr, size_t paddr, size_t flags);
size_t pag_umap(size_t vaddr);
size_t	pag_virt_to_phys(size_t vaddr);
size_t*	pag_get_pagedir();
size_t*	pag_get_task_pagedir(void* task);
void	pag_set_pagedir(size_t* pagedir);
size_t*	pag_alloc_pagedir();
void	pag_free_pagedir(size_t* pagedir);
void	pag_invalidate(size_t vaddr);

