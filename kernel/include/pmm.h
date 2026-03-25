#ifndef _PMM_H
#define _PMM_H

#include <kernel.h>

int pmm_init(void);
void* pmm_alloc(void);
int pmm_free(void*);

#endif
