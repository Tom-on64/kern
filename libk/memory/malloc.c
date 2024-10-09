#include <memory/virtual.h>
#include <memory/physical.h>
#include <memory/malloc.h>
#include <stdbool.h>
#include <stdint.h>

mallocNode_t* mallocListHead = NULL;
uint32_t mallocVirtualAddr = 0;
uint32_t mallocPhysicalAddr = 0;
uint32_t mallocPages = 0;

void mallocInit(uint32_t size) {
    mallocPages = size / PAGE_SIZE;
    if (size % PAGE_SIZE > 0) { mallocPages++; }

    mallocPhysicalAddr = (uint32_t)allocBlocks(mallocPages);
    mallocListHead = (mallocNode_t*)mallocVirtualAddr;
    
    // Map pages
    for (uint32_t i = 0, virtualAddr = mallocVirtualAddr; i < mallocPages; i++, virtualAddr += PAGE_SIZE) {
        mapPage((void*)(mallocPhysicalAddr + i * PAGE_SIZE), (void*)virtualAddr);

        ptEntry_t* page = getPage(virtualAddr);
        SET_ATTR(page, PTE_READ_WRITE);
    }

    if (mallocListHead) {
        mallocListHead->size = (mallocPages * PAGE_SIZE) - sizeof(mallocNode_t);
        mallocListHead->free = true;
        mallocListHead->next = NULL;
    }
}

void mallocSplit(mallocNode_t* node, uint32_t size) {
    if (node->size < size + sizeof(mallocNode_t)) { return; } // Can't split - node too small
    
    // Remainder node
    mallocNode_t* newNode = (mallocNode_t*)((void*)node + size + sizeof(mallocNode_t));
    newNode->size = node->size - size - sizeof(mallocNode_t);
    newNode->free = true;
    newNode->next = node->next;

    // Split node
    node->size = size;
    node->free = false;
    node->next = newNode;
}

void* mallocNextNode(uint32_t size) {
    if (size == 0) { return NULL; } // Why would you try that? (undefined behaviour i guess)
    
    mallocNode_t* current = mallocListHead;

    if (mallocListHead->size == 0) { mallocInit(size); }
    
    while (((current->size < size) || !current->free) && current->next != NULL) {
        current = current->next;
    }

    if (current->size == size) {
        current->free = false;
    } else if (current->size > size + sizeof(mallocNode_t)) {
        mallocSplit(current, size); // Modifies current into the right size and makes it point to the remainder
    } else { // There's no large enough node -> try to allocate more pages
        uint8_t pageCount = 1;
        while (current->size + pageCount * PAGE_SIZE < size + sizeof(mallocNode_t)) { 
            pageCount++;
        }

        // Allocate and map new pages
        uint32_t virtualAddr = mallocVirtualAddr + mallocPages * PAGE_SIZE;
        for (uint32_t i = 0; i < pageCount; i++) {
            ptEntry_t page = 0;
            uint32_t* temp = allocPage(&page);

            mapPage((void*)temp, (void*)virtualAddr);
            SET_ATTR(&page, PTE_READ_WRITE);
            virtualAddr += PAGE_SIZE;
            current->size += PAGE_SIZE;
            mallocPages++;
        }

        mallocSplit(current, size);
    }

    return (void*)current + sizeof(mallocNode_t);
}

void mallocMergeFree() {
    mallocNode_t* current = mallocListHead;

    while (current && current->next != NULL) {
        if (current->free && current->next->free) {
            current->size += (current->next->size) + sizeof(mallocNode_t);
            current->next = current->next->next;
        }
        current = current->next;
    }
}

void mallocFree(void* ptr) {
    mallocNode_t* current = mallocListHead;

    while (current->next) {
        if ((void*)current + sizeof(mallocNode_t) == ptr) {
            current->free = true;
            mallocMergeFree();
            break;
        }

        current = current->next;
    }
}

