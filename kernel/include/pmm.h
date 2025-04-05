#ifndef _PMM_H
#define _PMM_H

#include <bitmap.h>
#include <kernel.h>

extern bitmap_t pmm_bitmap;

int pmm_init(void);
void pmm_initRegion(size_t base, size_t len);
void pmm_deinitRegion(size_t base, size_t len);
void* pmm_alloc(size_t len);
void pmm_free(void* ptr, size_t len);

#endif
