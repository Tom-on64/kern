#include <bootloader.h>
#include <kernel.h>
#include <limine.h>
#include <paging.h>
#include <serial.h>

#include <pmm.h>

uint64_t* pmm_head = NULL;

int pmm_init(void) {
	size_t total_mem = 0;

	debugf("[pmm] Physical memory map:\n"); // We're gonna use the loop to also print the memmap
	for (size_t i = 0; i < bootloader.mm_entry_count; i++) {
		struct limine_memmap_entry* entry = bootloader.mm_entries[i];
		
		debugf(
			"\t%p - %p (%d bytes), type %d\n",
			entry->base, entry->base + entry->length, entry->length, entry->type
		);

		if (entry->type != LIMINE_MEMMAP_USABLE) continue;

		// Limine spec says that it'll be aligned, but just as a sanity check
		if (entry->base % PAGE_SIZE != 0) {
			size_t diff = PAGE_SIZE - (entry->base % PAGE_SIZE);
			debugf("\t Not 4 kiB aligned, shifting %p -> %p (%d bytes).\n", entry->base, entry->base + diff, diff);
			entry->base += diff;
			entry->length -= diff;
		}

		for (offset_t offset = 0; offset < entry->length; offset += PAGE_SIZE) {
			uint64_t* addr = TO_VIRT(entry->base) + offset;
			*addr = (uint64_t)pmm_head;
			pmm_head = addr;
			total_mem += PAGE_SIZE;
		}
	}

	debugf("[pmm] %d bytes of available memory.\n", total_mem);

	return 0;
}

void* pmm_alloc() {
	uint64_t* addr = pmm_head;
	pmm_head = (uint64_t*)(*addr);
	return TO_PHYS(addr);
}

void pmm_free(void* page) {
	uint64_t* addr = TO_VIRT(page);
	*addr = (uint64_t)pmm_head;
	pmm_head = addr;
}

