#ifndef VIRTUAL_H
#define VIRTUAL_H

#include <stdint.h>
#include <memory/physical.h>

#define PAGES_PER_TABLE 1024
#define TABLES_PER_DIRECTORY 1024
#define PAGE_SIZE 4096

#define PD_INDEX(_vAddress) ((_vAddress) >> 22)
#define PT_INDEX(_vAddress) (((_vAddress) >> 12) & 0x3ff)

typedef uint32_t ptEntry_t;
typedef uint32_t pdEntry_t;

typedef enum {
    PTE_PRESENT         = 0x01,
    PTE_READ_WRITE      = 0x02,
    PTE_USER            = 0x04,
    PTE_WRITE_THROUGH   = 0x08,
    PTE_CACHE_DISABLE   = 0x10,
    PTE_ACCESSED        = 0x20,
    PTE_DIRTY           = 0x40,
    PTE_PAT             = 0x80,
    PTE_GLOBAL          = 0x100,
    PTE_FRAME           = 0x7ffff000,
} PAGE_TABLE_FLAGS;

typedef enum {
    PDE_PRESENT         = 0x01,
    PDE_READ_WRITE      = 0x02,
    PDE_USER            = 0x04,
    PDE_WRITE_THROUGH   = 0x08,
    PDE_CACHE_DISABLE   = 0x10,
    PDE_ACCESSED        = 0x20,
    PDE_DIRTY           = 0x40, // Only with 4MB pages
    PDE_PAGE_SIZE       = 0x80, // Is page 4MB
    PDE_GLOBAL          = 0x100,
    PDE_PAT             = 0x2000,
    PDE_FRAME           = 0x7ffff000,
} PAGE_DIR_FLAGS;

typedef struct {
    ptEntry_t entries[PAGES_PER_TABLE];
} pageTable_t;

typedef struct {
    pdEntry_t entries[TABLES_PER_DIRECTORY];
} pageDirectory_t;

pageDirectory_t* currentPd = 0;

#endif
