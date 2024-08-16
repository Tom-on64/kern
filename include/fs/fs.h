#ifndef FS_FS_H
#define FS_FS_H

/*
 * For info, read the fs-spec.txt document inside of docs/
 */

#include <stdint.h>

// FS Definitions
#define FS_SECTOR_SIZE 512
#define FS_BLOCK_SIZE 4096

// Conversion Macros (beatiful)
#define BYTES2BLOCKS(_n)  ( (_n) == 0 ? 0 : \
                            (_n) <= (FS_BLOCK_SIZE) ? 1 : \
                            ((_n) / FS_BLOCK_SIZE) + (((_n) % FS_BLOCK_SIZE) > 0 ? 1 : 0) )

#define BYTES2SECTORS(_n) ( (_n) == 0 ? 0 : \
                            (_n) <= (FS_SECTOR_SIZE) ? 1 : \
                            ((_n) / FS_SECTOR_SIZE) + (((_n) % FS_SECTOR_SIZE) > 0 ? 1 : 0) )

// Filetypes
#define FT_FILE 0
#define FT_DIR  1

// FS Structures
// TODO: datetime_t?

typedef struct {
    uint8_t sectors[FS_BLOCK_SIZE / FS_SECTOR_SIZE][FS_SECTOR_SIZE];
} __attribute__ ((packed)) bootblock_t;

// Superblock - Padded to 4096 Bytes
typedef struct {
    // Constant data
    uint16_t deviceId;
    uint16_t extentsPerIndirect;
    uint32_t maxFileSize;
    uint16_t inodesPerSector;

    // INode data
    uint32_t inodeCount;
    uint32_t inodeBitmapBlock;
    uint32_t inodesStart;
    uint32_t inodeBlockCount;

    // Data data :)
    uint32_t dataBitmapBlock;
    uint32_t dataStart;
    uint32_t dataBlockCount;

    // Other
    uint32_t rootInodePtr;
    uint32_t firstUnreservedInode;
} __attribute__ ((packed)) superblock_t;

// Extent - 8 Bytes
typedef struct {
    uint32_t block;
    uint32_t length;
} __attribute__ ((packed)) extent_t;

// INode - 64 Bytes
typedef struct {
    uint32_t id;
    uint8_t type;
    uint32_t sizeBytes; // Size in bytes
    uint32_t sizeSectors; // Size in sectors
    uint8_t mode; // Access permisions
    extent_t extent[4];
    uint32_t singleIndirect;
    uint32_t doubleIndirect;

    uint8_t reserved[10];
} __attribute__ ((packed)) inode_t;

typedef struct {
    char name[60];
    uint32_t id;
} __attribute__ ((packed)) dirEntry_t;

#endif
