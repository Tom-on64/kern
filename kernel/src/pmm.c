#include <bootloader.h>
#include <kernel.h>
#include <limine.h>
#include <paging.h>
#include <serial.h>

#include <pmm.h>

uint64_t* pmm_head = NULL;

int pmm_init(void) {
	debugf("[pmm] Physical memory map:\n"); // We're gonna use the loop to also print the memmap
	for (size_t i = 0; i < bootloader.mm_entry_count; i++) {
		struct limine_memmap_entry* entry = bootloader.mm_entries[i];
		
		debugf(
			"\t0x%x - 0x%x (%d bytes), type %d\n",
			entry->base, entry->base + entry->length, entry->length, entry->type
		);

		if (entry->type != LIMINE_MEMMAP_USABLE) continue;

		for (offset_t offset = 0; offset < entry->length; offset += PAGE_SIZE) {
			uint64_t* addr = (uint64_t*)(entry->base + offset + bootloader.hhdm_offset);
			*addr = (uint64_t)pmm_head;
			pmm_head = addr;
		}
	}

	return 0;
}

void* pmm_alloc() {
	uint64_t* addr = pmm_head;
	pmm_head = (uint64_t*)(*addr);
	return addr - bootloader.hhdm_offset;
}

void pmm_free(void* page) {
	uint64_t* addr = (uint64_t*)(page - bootloader.hhdm_offset);
	*addr = (uint64_t)pmm_head;
	pmm_head = addr;
}

