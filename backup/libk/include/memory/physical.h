#ifndef _MEMORY_PHYSICAL_H
#define _MEMORY_PHYSICAL_H

#include <memory/addresses.h>
#include <string.h>
#include <stdint.h>

#define BLOCK_SIZE 4096 // 4kB
#define BLOCKS_PER_BYTE 8

typedef struct smapEntry_s {
    uint64_t baseAddress;
    uint64_t length;
    uint32_t type;
    uint32_t acpiAttr;
} __attribute__ ((packed)) smapEntry_t;

extern uint32_t* memoryMap;
extern uint32_t maxBlocks;
extern uint32_t usedBlocks;

void setBlock(uint32_t block);
void unsetBlock(uint32_t block);
uint8_t testBlock(uint32_t block);

int32_t findFreeBlocks(uint32_t blockCount);
void setupPhysicalMemoryManager(uint32_t startAddress, uint32_t size);
void initMemoryRegion(uint32_t baseAddress, uint32_t size);
void deinitMemoryRegion(uint32_t baseAddress, uint32_t size);
void* allocBlocks(uint32_t blockCount);
void freeBlocks(void* address, uint32_t blockCount);

#endif
