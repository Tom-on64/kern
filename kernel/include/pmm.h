#ifndef _PMM_H
#define _PMM_H

#include <multiboot.h>

#define PMM_BS	4096	// Block Size
#define PMM_BPB	8	// Blocks per byte

int pmm_init(void);
void pmm_initRegion(size_t base, size_t len);
void pmm_deinitRegion(size_t base, size_t len);
void* pmm_alloc(size_t blockCount);
void pmm_free(void* ptr, size_t blockCount);

#endif
