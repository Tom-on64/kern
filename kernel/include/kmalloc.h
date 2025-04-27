#ifndef _KMALLOC_H
#define _KMALLOC_H

void*	kmalloc(size_t size);
void*	kzalloc(size_t size);
void*	kcalloc(size_t count, size_t size);
void	kfree(void* ptr);

#endif
