// A utility to build the disk
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "include/fs/fs.h"

#define IMAGE_NAME "kern.iso"
#define BIN_DIR "./bin/"
#define DISK_SIZE  (2880 * 512) // 1.44MB

uint8_t nullBlock[FS_BLOCK_SIZE] = { 0 };

typedef struct {
    char name[60];
    uint32_t size;
    FILE* fp;
} fileInfo_t;

fileInfo_t files[] = { // TODO: Not have to list all of the files, loop through a directory or smth
    /* Bootloader */
    { BIN_DIR"stage1.bin" },
    { BIN_DIR"stage2.bin" },
    { BIN_DIR"stage3.bin" },
    /* Kernel stuff */
    { BIN_DIR"kernel.bin" },
    { BIN_DIR"term16n.fnt" },
    { BIN_DIR"testfont.fnt" },
    /* Userspace files */
    { "./src/fs/test.txt" },
    { BIN_DIR"calc.bin" },
    { BIN_DIR"edit.bin" },
};

superblock_t superblock = { 0 };

FILE* imagePtr;
uint32_t diskSize = DISK_SIZE;
uint32_t fileCount;
uint32_t fileBlocks = 0;
uint32_t dataBlocks;
uint32_t dataBitCount;

void writeBootblock();
void writeSuperblock();
void writeINodeBitmapBlocks();
void writeDataBitmapBlocks();
void writeINodeBlocks();
void writeDataBlocks();

uint32_t paddingBytes(uint32_t bytes, uint32_t bound) {
    if (bytes % bound == 0) return 0;
    return (bytes - (bytes % bound) + bound) - bytes;
}

int main(int argc, char** argv) { 
    imagePtr = fopen(IMAGE_NAME, "wb");

    fileCount = sizeof(files) / sizeof(files[0]);

    // Open files and get file info
    for (int i = 0; i < fileCount; i++) {
        files[i].fp = fopen(files[i].name, "rb");
        fseek(files[i].fp, 0, SEEK_END);
        files[i].size = ftell(files[i].fp);
        rewind(files[i].fp);
        fileBlocks += BYTES2BLOCKS(files[i].size);
    }

    printf("Building disk image %s...\n", IMAGE_NAME);
    printf("Block size: %d Bytes, Total disk blocks: %d (%d Bytes)\n\n", FS_BLOCK_SIZE, BYTES2BLOCKS(diskSize), diskSize);

    printf("%d files (2 hidden)\n", fileCount); // Stage 1 and 2 is hidden
    
    // Build disk image
    writeBootblock();
    writeSuperblock();
    writeINodeBitmapBlocks();
    writeDataBitmapBlocks();
    writeINodeBlocks();
    writeDataBlocks();

    uint32_t writtenBlocks = 4 + superblock.inodeBlockCount + superblock.dataBlockCount; 
    printf("\nWritten blocks: %d (%d Bytes)\n", writtenBlocks, writtenBlocks * FS_BLOCK_SIZE);
    printf("Free blocks: %d (%d Bytes)\n", BYTES2BLOCKS(diskSize) - writtenBlocks, diskSize - writtenBlocks * FS_BLOCK_SIZE);

    // Pad out to the whole disk size
    for (uint32_t i = 0; i < BYTES2BLOCKS(diskSize) - writtenBlocks; i++) {
        assert(fwrite(nullBlock, FS_BLOCK_SIZE, 1, imagePtr) == 1);
    }
    
    // Cleanup
    for (uint32_t i = 0; i < fileCount; i++) {
        fclose(files[i].fp);
    }
    fclose(imagePtr);

    return 0;
}

void writeBootblock() {
    bootblock_t bootBlock = { 0 };
    assert(fread(bootBlock.sectors[0], FS_SECTOR_SIZE, 1, files[0].fp) == 1); // File 0 - Stage 1

    for (uint8_t i = 1; i < BYTES2SECTORS(files[i].size); i++) {
        if (i > 7) {
            fprintf(stderr, "%s is too large, maximum size of bootblock is 7 sectors (%d Bytes)\n", files[1].name, 7 * FS_SECTOR_SIZE);
            exit(1);
        }

        assert(fread(bootBlock.sectors[i], FS_SECTOR_SIZE, 1, files[1].fp) == 1); // File 1 - Stage 2
    }
    
    // Write bootblock to disk
    for (uint8_t i = 0; i < FS_BLOCK_SIZE / FS_SECTOR_SIZE; i++) {
        assert(fwrite(bootBlock.sectors[i], FS_SECTOR_SIZE, 1, imagePtr) == 1);
    }
}

void writeSuperblock() {
    /* Block Layout
     * 0 - Boot
     * 1 - Super
     * 2 - INode Bitmap
     * 3 - Data Bitmap
     * 4 - First INode Block
     */

    // Constant
    superblock.deviceId = 0x01;
    superblock.extentsPerIndirect = FS_BLOCK_SIZE / sizeof(extent_t);
    superblock.maxFileSize = 0xffffffff;
    superblock.inodesPerSector = (FS_SECTOR_SIZE / sizeof(inode_t));

    // INode
    superblock.inodeCount = fileCount + 2; // 2 reserved INodes (0 - Invalid, 1 - Root)
    superblock.inodeBitmapBlock = 2;
    superblock.inodesStart = 4;
    superblock.inodeBlockCount = BYTES2BLOCKS(superblock.inodeCount * sizeof(inode_t));

    // Data
    dataBlocks = BYTES2BLOCKS(diskSize);

    superblock.dataBitmapBlock = 3;
    superblock.dataStart = superblock.inodesStart + superblock.inodeBlockCount;
    superblock.dataBlockCount = fileBlocks; // +1 for Root, -1 for Boot

    // Other
    superblock.rootInodePtr = 0; // Will be filled at runtime
    superblock.firstUnreservedInode = 3; // 0 - Invalid, 1 - Root, 2 - Bootloader

    assert(fwrite(&superblock, sizeof(superblock_t), 1, imagePtr) == 1);
    assert(fwrite(nullBlock, paddingBytes(sizeof(superblock_t), FS_BLOCK_SIZE), 1, imagePtr) == 1); // Padding
}

void writeINodeBitmapBlocks() {
    uint8_t temp[FS_SECTOR_SIZE] = { 0 };
    
    // Set number of bits = number of inodes
    *(uint32_t*)temp |= (2 << (superblock.inodeCount - 1)) - 1;
    assert(fwrite(temp, FS_SECTOR_SIZE, 1, imagePtr) == 1);

    // Padd out
    assert(fwrite(nullBlock, paddingBytes(FS_SECTOR_SIZE, FS_BLOCK_SIZE), 1, imagePtr) == 1);
}

void writeDataBitmapBlocks() {
    uint64_t bitChunk = 0;
    uint32_t totalBlocks = superblock.dataBlockCount / 32;
    uint32_t bytesWritten = 0;

    while (totalBlocks > 0) {
        bitChunk = 0xffffffff;
        totalBlocks--;
        assert(fwrite(&bitChunk, 4, 1, imagePtr) == 1);
        bytesWritten += 4;
    }

    bitChunk = (2 << ((superblock.dataBlockCount % 32) - 1)) - 1;
    assert(fwrite(&bitChunk, 4, 1, imagePtr) == 1);
    bytesWritten += 4;

    assert(fwrite(nullBlock, paddingBytes(bytesWritten, FS_BLOCK_SIZE), 1, imagePtr) == 1);
}

void writeINodeBlocks() {
    inode_t inode = { 0 };

    // INode 0 - Invalid
    assert(fwrite(&inode, sizeof(inode), 1, imagePtr) == 1);

    // INode 1 - Root directory
    inode.id = 1;
    inode.type = FT_DIR;
    inode.sizeBytes = sizeof(dirEntry_t) * fileCount;
    inode.sizeSectors = BYTES2SECTORS(inode.sizeBytes);
    // TODO: Mode

    inode.extent[0].block = superblock.dataStart;
    inode.extent[0].length = BYTES2BLOCKS(inode.sizeBytes);
        
    assert(fwrite(&inode, sizeof(inode), 1, imagePtr) == 1);

    // INode 2 - Bootloader (stage 3) followed by other INodes
    uint32_t block = superblock.dataStart + inode.extent[0].length;
    for (uint32_t i = 2; i < fileCount; i++) {
        memset(&inode, 0, sizeof(inode));
        inode.id = i;
        inode.type = FT_FILE;
        inode.sizeBytes = files[i].size;
        inode.sizeSectors = BYTES2SECTORS(inode.sizeBytes);

        inode.extent[0].block = block,
        inode.extent[0].length = BYTES2BLOCKS(inode.sizeBytes);

        assert(fwrite(&inode, sizeof(inode), 1, imagePtr) == 1);
        
        block += inode.extent[0].length;
    }
    
    assert(fwrite(nullBlock, paddingBytes(fileCount * sizeof(inode_t), FS_BLOCK_SIZE), 1, imagePtr) == 1);
}

void writeDataBlocks() {
    uint8_t sector[FS_SECTOR_SIZE] = { 0 };
    dirEntry_t dirEntry = {
        .id = 1,
        .name = ".",
    };

    assert(fwrite(&dirEntry, sizeof(dirEntry), 1, imagePtr) == 1);
    strcpy(dirEntry.name, "..");
    assert(fwrite(&dirEntry, sizeof(dirEntry), 1, imagePtr) == 1);

    for (uint32_t i = 2; i < fileCount; i++) {
        dirEntry.id = i;
        strcpy(dirEntry.name, strrchr(files[i].name, '/') + 1); // We skip the './build/'
        assert(fwrite(&dirEntry, sizeof(dirEntry), 1, imagePtr) == 1);
    }

    // Pad out
    assert(fwrite(nullBlock, paddingBytes(fileCount * sizeof(dirEntry_t), FS_BLOCK_SIZE), 1, imagePtr) == 1);

    // Write actual file data
    for (uint32_t i = 2; i < fileCount; i++) {
        uint32_t blockCount = BYTES2BLOCKS(files[i].size);
        uint32_t bytesWritten = 0;

        printf("%s, %d Bytes -> %d Blocks\n", files[i].name, files[i].size, blockCount);

        for (uint32_t j = 0; j < blockCount * (FS_BLOCK_SIZE / FS_SECTOR_SIZE); j++) {
            uint32_t byteCount = fread(sector, 1, sizeof(sector), files[i].fp);
            fwrite(sector, 1, byteCount, imagePtr);

            bytesWritten += byteCount;

            if (byteCount < sizeof(sector)) break; // EOF
        }

        memset(sector, 0, sizeof(sector));

        // Padd out block
        fwrite(nullBlock, paddingBytes(bytesWritten, FS_BLOCK_SIZE), 1, imagePtr);
    }
}

