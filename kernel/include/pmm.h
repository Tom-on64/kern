#ifndef _PMM_H
#define _PMM_H

#include <bitmap.h>
#include <kernel.h>

extern bitmap_t pmm_bitmap;

int pmm_init(void);
uint32_t pmm_alloc(void);
void pmm_free(uint32_t page);

#endif
