#ifndef MEMORY_VIRTUAL_H
#define MEMORY_VIRTUAL_H

#include <stdint.h>
#include <memory/physical.h>
#include <string.h>
#include <memory/addresses.h>

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

pageDirectory_t* currentPd = NULL;

ptEntry_t* getPtEntry(pageTable_t* pt, uint32_t virtualAddress) {
    if (pt) return &pt->entries[PT_INDEX(virtualAddress)];
    return NULL;
}

pdEntry_t* getPdEntry(pageDirectory_t* pd, uint32_t virtualAddress) {
    if (pd) return &pd->entries[PT_INDEX(virtualAddress)]; // It it supposed to be PT_INDEX, don't worry
    return NULL;
}

ptEntry_t* getPage(uint32_t virtualAddress) {
    pageDirectory_t* pd = currentPd;
    pdEntry_t* entry = &pd->entries[PD_INDEX(virtualAddress)];
    pageTable_t* table = (pageTable_t*)PAGE_PHYS_ADDRESS(entry);
    ptEntry_t* page = &table->entries[PT_INDEX(virtualAddress)];
    return page;
}

void* allocPage(ptEntry_t* page) {
    void* block = allocBlocks(1);
    if (block) {
        SET_FRAME(page, (uint32_t)block);
        SET_ATTR(page, PTE_PRESENT);
    }

    return block;
}

void freePage(ptEntry_t* page) {
    void* address = (void*)PAGE_PHYS_ADDRESS(page);
    if (address) {
        freeBlocks(address, 1);
    }

    CLEAR_ATTR(page, PTE_PRESENT);
}

uint8_t setPageDirectory(pageDirectory_t* pd) {
    if (!pd) { return 1; } // Nuh uh
    currentPd = pd;
    
    // cr3 (Control register 3) - current page dir address
    __asm__ volatile("movl %%eax, %%cr3" : : "a"(currentPd));

    return 0; // Success
}

void flushTlbEntry(uint32_t virtualAddress) {
    __asm__ volatile("cli; invlpg (%0); sti" : : "r"(virtualAddress));
}

uint8_t mapPage(void* physicalAddr, void* virtualAddr) {
    pageDirectory_t* pd = currentPd;
    pdEntry_t* entry = &pd->entries[PD_INDEX((uint32_t)virtualAddr)];

    if (!TEST_ATTR(entry, PTE_PRESENT)) {
        // Assuming that the page is not allocated since it's not present
        pageTable_t* table = (pageTable_t*)allocBlocks(1);
        if (!table) return 1; // Out of memory!!

        // Clear page table (may be slow?)
        memset(table, 0, sizeof(pageTable_t));

        pdEntry_t* entry = &pd->entries[PD_INDEX((uint32_t)virtualAddr)];
        // Enable attributes and map in table
        SET_ATTR(entry, PDE_PRESENT);
        SET_ATTR(entry, PDE_READ_WRITE);
        SET_FRAME(entry, (uint32_t)table);
    }

    pageTable_t* table = (pageTable_t*)PAGE_PHYS_ADDRESS(entry);
    ptEntry_t* page = &table->entries[PT_INDEX((uint32_t)virtualAddr)];

    SET_ATTR(page, PTE_PRESENT);
    SET_FRAME(page, (uint32_t)physicalAddr);

    return 0; // Success
}

void unmapPage(void* virtualAddress) {
    ptEntry_t* page = getPage((uint32_t)virtualAddress);
    
    SET_FRAME(page, 0);
    CLEAR_ATTR(page, PDE_PRESENT);
}

uint8_t setupVirtualMemoryManager() {
    // Create a default page dir
    pageDirectory_t* dir = (pageDirectory_t*)allocBlocks(3);
    
    if (!dir) { return 1; } // Out of memory

    memset(dir, 0, sizeof(pageDirectory_t));
    for (uint32_t i = 0; i < 1024; i++) {
        dir->entries[i] = 0x02; // Supervisor, r/w, not present
    }

    // Alloc default page table
    pageTable_t* table = (pageTable_t*)allocBlocks(1);
    if (!table) { return 1; } // Out of memory
    
    // Alloc a 3GB page
    pageTable_t* table3gb = (pageTable_t*)allocBlocks(1);
    if (!table) { return 1; } // Out of memory

    memset(table, 0, sizeof(pageTable_t));
    memset(table3gb, 0, sizeof(pageTable_t));

    // Identity map first 4MB
    for (uint32_t i = 0, frame = 0, virt = 0; i < 1024; i++, frame += PAGE_SIZE, virt += PAGE_SIZE) {
        ptEntry_t page = 0;
        SET_ATTR(&page, PTE_PRESENT);
        SET_ATTR(&page, PTE_READ_WRITE);
        SET_FRAME(&page, frame);

        table3gb->entries[PT_INDEX(virt)] = page;
    }

    // Map kernel to 3GB+
    for (uint32_t i = 0, frame = KERNEL_LOC, virt = 0xc0000000; i < 1024; i++, frame += PAGE_SIZE, virt += PAGE_SIZE) {
        ptEntry_t page = 0;
        SET_ATTR(&page, PTE_PRESENT);
        SET_ATTR(&page, PTE_READ_WRITE);
        SET_FRAME(&page, frame);

        table->entries[PT_INDEX(virt)] = page;
    }
    
    pdEntry_t* entry = &dir->entries[PD_INDEX(0xc0000000)];
    SET_ATTR(entry, PDE_PRESENT);
    SET_ATTR(entry, PDE_READ_WRITE);
    SET_FRAME(entry, (uint32_t)table);

    pdEntry_t* entry2 = &dir->entries[PD_INDEX(0x00000000)];
    SET_ATTR(entry2, PDE_PRESENT);
    SET_ATTR(entry2, PDE_READ_WRITE);
    SET_FRAME(entry2, (uint32_t)table3gb);

    setPageDirectory(dir);

    // ENABLE PAGING!!! Set PG bit 31 and PE bit 0 in cr0
    __asm__ volatile("movl %cr0, %eax; orl $0x80000001, %eax; movl %eax, %cr0");
    return 0; // Success
}

#endif
