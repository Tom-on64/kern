#ifndef _PMM_H
#define _PMM_H

#include <bitmap.h>
#include <kernel.h>

extern bitmap_t pmm_bitmap;

int pmm_init(void);
void* pmm_alloc(void);
void pmm_free(void* page);

#endif
