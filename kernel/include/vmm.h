#ifndef _VMM_H
#define _VMM_H

#include <kernel.h>
#include <bitmap.h>

extern bitmap_t vmm_bitmap;

int   vmm_init(void);
void* vmm_alloc(size_t pages);
void  vmm_free(void* ptr, size_t pages);

#endif
