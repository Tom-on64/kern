// A utility to build the disk
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "include/fs/fs.h"

#define IMAGE_NAME "kern.iso"
#define DISK_SIZE  (2880 * 512) // 1.44MB

uint8_t nullBlock[FS_BLOCK_SIZE] = { 0 };

typedef struct {
    char name[60];
    uint32_t size;
    FILE* fp;
} fileInfo_t;

fileInfo_t files[] = { // TODO: Not have to list all of the files, loop through a directory or smth
    { "./build/stage1.bin" },
    { "./build/stage2.bin" },
    { "./build/stage3.bin" },
    { "./build/kernel.bin" },
    { "./build/term16n.fnt" },
    { "./build/testfont.fnt" },
    { "./src/fs/test.txt" },
    { "./build/calc.bin" },
    { "./build/editor.bin" },
};

superBlock_t superblock = { 0 };

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
        fileBlocks += bytesToBlocks(files[i].size);
    }

    printf("Building disk image %s...\n", IMAGE_NAME);
    printf("Block size: %d Bytes, Total disk blocks: %d\n", FS_BLOCK_SIZE, bytesToBlocks(diskSize));
    printf("Used blocks: %d, Free blocks %d\n\n", fileBlocks, bytesToBlocks(diskSize) - fileBlocks);
    printf("%d files (2 hidden)\n", fileCount); // Stage 1 and 2 is hidden

    // Build disk image
    writeBootblock();
    writeSuperblock();
    writeINodeBitmapBlocks();
    writeDataBitmapBlocks();
    writeINodeBlocks();
    writeDataBlocks();

    // Pad out to the whole disk size
    uint32_t writtenBlocks = 2 + superblock.inodeBitmapBlockCount + superblock.dataBitmapBlockCount + superblock.inodeBlockCount + superblock.dataBlockCount; 
    for (uint32_t i = 0; i < bytesToBlocks(diskSize) - writtenBlocks; i++) {
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
    bootBlock_t bootBlock = { 0 };
    assert(fread(bootBlock.sectors[0], FS_SECTOR_SIZE, 1, files[0].fp) == 1); // File 0 - Stage 1

    for (uint8_t i = 1; i < bytesToSectors(files[i].size); i++) {
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
    superblock.inodeCount = fileCount + 2;  // 2 reserved INodes (0 - Invalid, 1 - Root)
    superblock.firstInodeBitmapBlock = 2;   // Block 0 - Boot, Block 1 - Superblock
    superblock.inodeBitmapBlockCount = superblock.inodeCount / (FS_BLOCK_SIZE * 8) + ((superblock.inodeCount % (FS_BLOCK_SIZE * 8) > 0) ? 1 : 0);
    superblock.firstDataBitmapBlock = superblock.firstInodeBlock + superblock.inodeBitmapBlockCount;
    
    dataBlocks = bytesToBlocks(diskSize);
    dataBitCount = (dataBlocks - superblock.firstDataBitmapBlock);

    superblock.dataBitmapBlockCount = dataBitCount / (FS_BLOCK_SIZE * 8) + ((dataBitCount % (FS_BLOCK_SIZE * 8) > 0) ? 1 : 0);
    superblock.firstInodeBlock = superblock.firstDataBitmapBlock + superblock.dataBitmapBlockCount;
    superblock.inodeBlockCount = bytesToBlocks(superblock.inodeCount * sizeof(inode_t));
    superblock.firstDataBlock = superblock.firstInodeBlock + superblock.inodeBlockCount;
    superblock.dataBlockCount = (fileBlocks - 2) + 1; // 1 Block for root dir (-1 for boot block)
    superblock.maxFileSize = 0xffffffff; 
    superblock.blockSize = FS_BLOCK_SIZE;
    superblock.inodeSize = sizeof(inode_t);
    superblock.rootInodePtr = 0; // Gets setup at runtime
    superblock.inodesPerBlock = FS_BLOCK_SIZE / sizeof(inode_t);
    superblock.directExtentsPerInode = 4;
    superblock.extentsPerIndirectBlock = FS_BLOCK_SIZE / sizeof(extent_t);
    superblock.firstFreeInodeBit = superblock.inodeCount;
    superblock.firstFreeDataBit = (fileBlocks - 2) + 1; // +1 for root dir
    superblock.deviceNumber = 0x01;
    superblock.firstUnreservedInode = 3; // 0 - Invalid; 1 - Root dir; 2 - Bootloader INode
    
    assert(fwrite(&superblock, sizeof(superBlock_t), 1, imagePtr) == 1);
    assert(fwrite(&nullBlock, paddingBytes(sizeof(superBlock_t), FS_BLOCK_SIZE), 1, imagePtr) == 1); // Padding
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
    fsDatetime_t defaultTimestamp = {
        .second = 12,
        .minute = 12,
        .hour   = 12,
        .day    = 20,
        .month  = 4,
        .year   = 2024,
    };

    // INode 0 - Invalid
    assert(fwrite(&inode, sizeof(inode), 1, imagePtr) == 1);

    // INode 1 - Root directory
    inode = (inode_t) { // OOP In C???
        .id = 1,
        .filetype = FILETYPE_DIR,
        .sizeBytes = sizeof(dirEntry_t) * fileCount,
        .sizeSectors = bytesToSectors(inode.sizeBytes),
        .timestamp = defaultTimestamp,
        .extent[0] = (extent_t) {
            .firstBlock = superblock.firstDataBlock,
            .length = bytesToBlocks(inode.sizeBytes),
        },
    };

    // INode 2 - Bootloader (stage 3) followed by other INodes
    uint32_t id = 2;
    uint32_t block = superblock.firstDataBlock + inode.extent[0].length;
    for (uint32_t i = 2; i < fileCount; i++) {
        memset(&inode, 0, sizeof(inode));
        inode.id = id++;
        inode.filetype = FILETYPE_FILE;
        inode.sizeBytes = files[i].size;
        inode.sizeSectors = bytesToSectors(inode.sizeBytes);
        inode.timestamp = defaultTimestamp;
        inode.extent[0] = (extent_t) {
            .firstBlock = block,
            .length = bytesToBlocks(inode.sizeBytes),
        };

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

    // Padd out
    assert(fwrite(nullBlock, paddingBytes(fileCount * sizeof(dirEntry_t), FS_BLOCK_SIZE), 1, imagePtr) == 1);

    // Write actual file data
    uint32_t bytesWritten = 0;
    for (uint32_t i = 2; i < fileCount; i++) {
        uint32_t blockCount = bytesToBlocks(files[i].size);
        printf("%s, %d Bytes -> %d Blocks\n", files[i].name, files[i].size, blockCount);
        bytesWritten = 0;

        for (uint32_t j = 0; j < blockCount * (FS_BLOCK_SIZE / FS_SECTOR_SIZE); j++) {
            uint32_t byteCount = fread(sector, 1, sizeof(sector), files[i].fp);
            fwrite(sector, 1, sizeof(sector), imagePtr);
            bytesWritten += byteCount;

            if (byteCount < sizeof(sector)) break; // EOF
        }

        memset(sector, 0, sizeof(sector));

        // Padd out block
        fwrite(nullBlock, paddingBytes(bytesWritten, FS_BLOCK_SIZE), 1, imagePtr);
    }
}

