#ifndef MEMORY_VIRTUAL_H
#define MEMORY_VIRTUAL_H

#include <stdint.h>
#include <memory/physical.h>

#define PAGES_PER_TABLE 1024
#define TABLES_PER_DIRECTORY 1024
#define PAGE_SIZE 4096

#define PD_INDEX(_vAddress) ((_vAddress) >> 22)
#define PT_INDEX(_vAddress) (((_vAddress) >> 12) & 0x3ff)
#define PAGE_PHYS_ADDRESS(_dirEntry) (*(_dirEntry) & ~ 0xfff)
#define SET_ATTR(_entry, _attr) (*(_entry) |= (_attr))
#define CLEAR_ATTR(_entry, _attr) (*(_entry) &= ~(_attr))
#define TEST_ATTR(_entry, _attr) (*(_entry) & (_attr))
#define SET_FRAME(_entry, _address) (*(_entry) = (*(_entry) & ~0xfffff000) | (_address))

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

extern pageDirectory_t* currentPd;

ptEntry_t* getPtEntry(pageTable_t* pt, uint32_t virtualAddress);
pdEntry_t* getPdEntry(pageDirectory_t* pd, uint32_t virtualAddress);
ptEntry_t* getPage(uint32_t virtualAddress);
void* allocPage(ptEntry_t* page);
void freePage(ptEntry_t* page);
uint8_t setPageDirectory(pageDirectory_t* pd);
void flushTlbEntry(uint32_t virtualAddress);
uint8_t mapPage(void* physicalAddr, void* virtualAddr);
void unmapPage(void* virtualAddress);
uint8_t setupVirtualMemoryManager();

#endif
