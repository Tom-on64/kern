#ifndef _PMM_H
#define _PMM_H

#include <kernel.h>

int pmm_init(void);
void* pmm_alloc(void);
void pmm_free(void*);

#endif
