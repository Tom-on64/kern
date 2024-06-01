#ifndef FS_H
#define FS_H

#include <stdint.h>

#define FS_BLOCK_SIZE  4096
#define FS_SECTOR_SIZE 512

#define FILETYPE_FILE 0
#define FILETYPE_DIR  1

// Filesystem structures
typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t reserved;
} __attribute__ ((packed)) fsDatetime_t;

typedef struct {
    uint8_t sectors[FS_BLOCK_SIZE / FS_SECTOR_SIZE][FS_SECTOR_SIZE];
} __attribute__ ((packed)) bootBlock_t;

typedef struct {
    uint32_t inodeCount;

    uint16_t firstInodeBitmapBlock;
    uint16_t firstDataBitmapBlock;
    uint16_t inodeBitmapBlockCount;
    uint16_t dataBitmapBlockCount;
    uint32_t firstInodeBlock;
    uint32_t firstDataBlock;
    uint16_t inodeBlockCount;
    uint16_t dataBlockCount;

    uint32_t maxFileSize; // in bytes
    uint16_t blockSize; // in bytes
    uint8_t inodeSize; // in bytes
    
    uint32_t rootInodePtr;
    uint8_t inodesPerBlock;
    uint8_t directExtentsPerInode;
    uint16_t extentsPerIndirectBlock;
    uint32_t firstFreeInodeBit;
    uint32_t firstFreeDataBit;
    uint16_t deviceNumber;
    uint8_t firstUnreservedInode;

    uint8_t reserved[14];
} __attribute__ ((packed)) superBlock_t;

typedef struct {
    uint32_t firstBlock;
    uint32_t length;
} __attribute__ ((packed)) extent_t;

typedef struct {
    uint32_t id;        // Idetifier
    uint8_t filetype;
    uint32_t sizeBytes;
    uint32_t sizeSectors;
    fsDatetime_t timestamp;
    extent_t extent[4];
    uint32_t singleIndirectBlock;
    uint32_t doubleIndirectBlock;
    uint8_t refCount;
    uint8_t reserved[2];
} __attribute__ ((packed)) inode_t;

typedef struct {
    uint32_t id;        // Matching ID with INode
    char name[60];
} __attribute__ ((packed)) dirEntry_t;

// Utility functions
uint32_t bytesToBlocks(uint32_t bytes) {
    if (bytes == 0) return 0;
    if (bytes <= FS_BLOCK_SIZE) return 1;
    return (bytes / FS_BLOCK_SIZE) + ((bytes % FS_BLOCK_SIZE) > 0 ? 1 : 0);
}

uint32_t bytesToSectors(uint32_t bytes) {
    if (bytes == 0) return 0;
    if (bytes <= FS_SECTOR_SIZE) return 1;
    return (bytes / FS_SECTOR_SIZE) + ((bytes % FS_SECTOR_SIZE) > 0 ? 1 : 0);
}

#endif
