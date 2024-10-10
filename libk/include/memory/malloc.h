#ifndef MEMORY_MALLOC_H
#define MEMORY_MALLOC_H

#include <memory/virtual.h>
#include <memory/physical.h>
#include <stdbool.h>
#include <stdint.h>

// Singly linked list!
typedef struct mallocNode_s {
    uint32_t size;
    bool free;
    struct mallocNode_s* next;
} mallocNode_t;

extern mallocNode_t* mallocListHead;
extern uint32_t mallocVirtualAddr;
extern uint32_t mallocPhysicalAddr;
extern uint32_t mallocPages;

void mallocInit(uint32_t size);
void mallocSplit(mallocNode_t* node, uint32_t size);
void* mallocNextNode(uint32_t size);
void mallocMergeFree();
void mallocFree(void* ptr);

#endif
