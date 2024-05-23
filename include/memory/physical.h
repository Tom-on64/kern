#ifndef PHYSICAL_MEM_MANAGER_H
#define PHYSICAL_MEM_MANAGER_H

#include <string.h>
#include <stdint.h>

#define BLOCK_SIZE 4096 // 4kB
#define BLOCKS_PER_BYTE 8
#define SMAP_ADDRESS 0x8500

typedef struct smapEntry_s {
    uint64_t baseAddress;
    uint64_t length;
    uint32_t type;
    uint32_t acpiAttr;
} __attribute__ ((packed)) smapEntry_t;

static uint32_t* memoryMap = 0;
static uint32_t maxBlocks;
static uint32_t usedBlocks = 0;

void setBlock(uint32_t block) {
    // Set the right bit to 1
    memoryMap[block/32] |= (1 << (block % 32));
}

void unsetBlock(uint32_t block) {
    // Set the right bit to 0
    memoryMap[block/32] &= ~(1 << (block % 32));
}

uint8_t testBlock(uint32_t block) {
    // Check the bit
    return memoryMap[block/32] & (1 << (block % 32)) ? 1 : 0;
}

// TODO: Refactor whatever the fuck this is
int32_t findFreeBlocks(uint32_t blockCount) {
    if (blockCount == 0) { // Error
        return -1;
    }

    uint32_t maxBlockEntries = maxBlocks / 32;
    for (uint32_t i = 0; i < maxBlockEntries; i++) {
        if (memoryMap[i] != 0xffffffff) {
            for (uint8_t j = 0; j < 32; j++) {
                uint32_t block = 1 << j;

                if (!(memoryMap[i] & block)) {
                    // uint32_t startBlock = i * 32 + block;
                    // uint32_t freeBlocks = 0;

                    for (uint32_t count = i, freeBlocks = 0; count < blockCount; count++) {
                        if (j + count > 31 && i + 1 <= maxBlocks / 32) {
                            if (!(memoryMap[i+1] & (1 << (j + count)))) {
                                freeBlocks++;
                            }
                        } else {
                            if (!(memoryMap[i] & (1 << (j + count)))) {
                                freeBlocks++;
                            }
                        }

                        if (freeBlocks == blockCount) {
                            return i * 32 + j;
                        }
                    }
                }
            }
        }
    }

    return -1; // No free memory found :/
}

void setupPhysicalMemoryManager(uint32_t startAddress, uint32_t size) {
    memoryMap = (uint32_t*)startAddress;
    maxBlocks = size / BLOCK_SIZE;
    usedBlocks = maxBlocks;

    memset((char*)memoryMap, 0xff, maxBlocks / BLOCKS_PER_BYTE);
}

void initMemoryRegion(uint32_t baseAddress, uint32_t size) {
    uint32_t align = baseAddress / BLOCK_SIZE;
    uint32_t blockCount = size / BLOCK_SIZE;
    
    while (blockCount-- > 0) {
        unsetBlock(align++);
        usedBlocks--;
    }

    setBlock(0); // Make sure we don't override the first block that contains IDT, BIOS stuff etc.
}

void deinitMemoryRegion(uint32_t baseAddress, uint32_t size) {
    uint32_t align = baseAddress / BLOCK_SIZE;
    uint32_t blockCount = size / BLOCK_SIZE;
    
    while (blockCount-- > 0) {
        setBlock(align++);
        usedBlocks++;
    }
}

void* allocBlocks(uint32_t blockCount) {
    if ((maxBlocks - usedBlocks) < blockCount) {
        return NULL;
    }

    int32_t startBlock = findFreeBlocks(blockCount);
    if (startBlock == -1) {
        return NULL;
    }

    for (uint32_t i = 0; i < blockCount; i++) {
        setBlock(startBlock + i);
        usedBlocks++;
    }

    return (void*)(startBlock * BLOCK_SIZE);
}

void freeBlocks(void* address, uint32_t blockCount) {
    uint32_t startBlock = (uint32_t)(address) / BLOCK_SIZE;
    
    for (uint32_t i = 0; i < blockCount; i++) {
        unsetBlock(startBlock + i);
        usedBlocks--;
    }

    setBlock(0); // Make sure that block 0 is always set
}

#endif
