#ifndef _PMM_H
#define _PMM_H

#include <kernel.h>

extern size_t pmm_total_pages;
extern size_t pmm_free_pages;

int pmm_init(void);
void* pmm_alloc(void);
int pmm_free(void*);

#endif
