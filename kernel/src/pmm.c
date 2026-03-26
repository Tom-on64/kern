#include <bootloader.h>
#include <kernel.h>
#include <limine.h>
#include <paging.h>
#include <errno.h>

#include <pmm.h>

uint64_t* pmm_head = NULL;
size_t pmm_total_mem = 0;

int pmm_init(void) {
	pr_info("[pmm] Physical memory map:\n"); // We're gonna use the loop to also print the memmap
	for (size_t i = 0; i < bootloader.mm_entry_count; i++) {
		struct limine_memmap_entry* entry = bootloader.mm_entries[i];
		
		pr_info(
			"\t%p - %p (%d bytes), type %d\n",
			entry->base, entry->base + entry->length, entry->length, entry->type
		);

		if (entry->type != LIMINE_MEMMAP_USABLE) continue;

		// Limine spec says that it'll be aligned, but just as a sanity check
		if (entry->base % PAGE_SIZE != 0) {
			size_t diff = PAGE_SIZE - (entry->base % PAGE_SIZE);
			pr_info("\t Not 4 kiB aligned, shifting %p -> %p (%d bytes).\n", entry->base, entry->base + diff, diff);
			entry->base += diff;
			entry->length -= diff;
		}

		for (offset_t offset = 0; offset < entry->length; offset += PAGE_SIZE) {
			uint64_t* addr = TO_VIRT(entry->base) + offset;
			*addr = (uint64_t)pmm_head;
			pmm_head = addr;
			pmm_total_mem += PAGE_SIZE;
		}
	}

	pr_info("[pmm] %d bytes of available memory.\n", pmm_total_mem);
	if (pmm_total_mem == 0) return -ENOMEM;

	return SUCCESS;
}

void* pmm_alloc() {
	if (pmm_head == NULL) return ERR_PTR(-ENOMEM);

	uint64_t* addr = pmm_head;
	if ((uintptr_t)addr % PAGE_SIZE != 0) panic("pmm_head corrupted.");
	pmm_head = (uint64_t*)(*addr);

	return TO_PHYS(addr);
}

int pmm_free(void* page) {
	if (page == NULL || IS_ERR(page)) return -EFAULT;
	if ((uintptr_t)page % PAGE_SIZE != 0) return -EALIGN;

	uint64_t* addr = TO_VIRT(page);
	*addr = (uint64_t)pmm_head;
	pmm_head = addr;

	return SUCCESS;
}

