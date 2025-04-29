#include <kernel.h>
#include <paging.h>
#include <serial.h>
#include <string.h>
#include <pmm.h>

#include <kmalloc.h>

struct kmalloc_node* kmalloc_head = NULL;
size_t kmalloc_pages;
size_t kmalloc_base;

int kmalloc_init(void) {
	kmalloc_base = KERNEL_HEAP_BASE;
	kmalloc_pages = KMALLOC_MIN_PAGES;

	for (size_t i = 0; i < kmalloc_pages; i++) {
		uint32_t paddr = (uint32_t)pmm_alloc();

		if (paddr == 0) {
			kmalloc_pages = i;
			break;
		}

		pag_map(kmalloc_base + i * PAGE_SIZE, paddr, 0);
	}

	kmalloc_head = (struct kmalloc_node*)kmalloc_base;

	if (kmalloc_pages < KMALLOC_MIN_PAGES) return 1;

	kmalloc_head->size = (kmalloc_pages * PAGE_SIZE) - sizeof(*kmalloc_head);
	kmalloc_head->free = 1;
	kmalloc_head->next = NULL;

	debugf("[kmalloc] kmalloc() initialized.\n");

	return 0;
}

void kmalloc_split(struct kmalloc_node* node, size_t size) {
	if (node->size < size + sizeof(struct kmalloc_node)) return;

	struct kmalloc_node* remainder = 
		(struct kmalloc_node*)((uint32_t)node + size + sizeof(struct kmalloc_node));
	remainder->size = node->size - size - sizeof(struct kmalloc_node);
	remainder->free = 1;
	remainder->next = node->next;

	node->size = size;
	node->free = 0;
	node->next = remainder;
}

void kmalloc_merge() {
	struct kmalloc_node* current = kmalloc_head;

	while (current != NULL && current->next != NULL) {
		if (current->free && current->next->free) {
			current->size += current->next->size + sizeof(struct kmalloc_node);
			current->next = current->next->next;
			continue;
		}

		current = current->next;
	}
}

void* kmalloc(size_t size) {
	if (size == 0) return NULL; // Why would you...

	struct kmalloc_node* current = kmalloc_head;

	while ((current->size <= size || !current->free) && current->next != NULL) {
		current = current->next;
	}

	if (current->size == size) current->free = 0;
	else if (current->size > size) kmalloc_split(current, size);
	else { // Allocate more pages
		size_t found_size = current->free ? current->size : 0;
		size_t page_count = 1;
		found_size += PAGE_SIZE;

		while (found_size < size + sizeof(struct kmalloc_node)) {
			page_count++;
			found_size += PAGE_SIZE;
		}

		uint32_t vaddr = kmalloc_base + kmalloc_pages * PAGE_SIZE;
		for (size_t i = 0; i < page_count; i++) {
			// WARN: This could be a memory leak! It doesn't free after allocating :/
			uint32_t paddr = (uint32_t)pmm_alloc();
			if (paddr == 0) return NULL;

			pag_map(vaddr + i * PAGE_SIZE, paddr, 0);
			kmalloc_pages++;
		}

		if (current->free) {
			current->size += PAGE_SIZE * page_count;
		} else {
			current->next = (struct kmalloc_node*)vaddr;
			current = current->next;

			current->size = PAGE_SIZE * page_count;
			current->free = 1;
			current->next = NULL;
		}

		return kmalloc(size);
	}

	return (void*)current + sizeof(struct kmalloc_node);
}

void* kzalloc(size_t size) {
	void* ptr = kmalloc(size);
	if (ptr == NULL) return NULL;
	memset(ptr, 0, size);
	return ptr;
}

void* kcalloc(size_t size, size_t count) {
	size_t siz = size * count;
	void* ptr = kmalloc(siz);
	if (ptr == NULL) return NULL;
	memset(ptr, 0, siz);
	return ptr;
}

void* krealloc(size_t size, void* ptr) {
	void* new = kmalloc(size);

	struct kmalloc_node* current = kmalloc_head;
	while (current != NULL) {
		if ((void*)current + sizeof(struct kmalloc_node) == ptr) break;
		current = current->next;
	}

	// Tried to realloc non-existent buffer
	if (current == NULL) return NULL;
	if (size > current->size) size = current->size;
	memcpy(ptr, new, size);
	kfree(ptr);

	return new;
}

void kfree(void* ptr) {
	struct kmalloc_node* current = kmalloc_head;

	while (current != NULL) {
		if ((void*)current + sizeof(struct kmalloc_node) == ptr) {
			current->free = 1;
			kmalloc_merge();
			break;
		}
		current = current->next;
	}
}

