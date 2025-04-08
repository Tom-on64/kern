#ifndef _PMM_H
#define _PMM_H

#include <bitmap.h>
#include <kernel.h>

extern bitmap_t pmm_bitmap;

int pmm_init(void);
void* pmm_allocPage(void);
void pmm_freePage(void* page);

#endif
