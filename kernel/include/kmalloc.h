#ifndef _KALLOC_H
#define _KALLOC_H

#include <kernel.h>

// TODO: Set a better minimum that just a random number
#define KMALLOC_MIN_PAGES 4

// kmalloc() singly-linked list node
struct kmalloc_node {
	size_t size;
	int free;
	struct kmalloc_node* next;
};

extern struct kmalloc_node* kmalloc_head;

int   kmalloc_init(void);
void* kmalloc(size_t size);
void* kzalloc(size_t size);
void* kcalloc(size_t size, size_t count);
void* krealloc(size_t size, void* ptr);
void  kfree(void* ptr);

#endif
