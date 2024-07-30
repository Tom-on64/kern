#ifndef IMPL_H
#define IMPL_H

#include <disk/disk.h>
#include <fs/fs.h>
#include <memory/addresses.h>
#include <stdint.h>
#include <string.h>

char* currentDir;
inode_t currentDirINode;

char tempSector[FS_SECTOR_SIZE];
char tempBlock[FS_BLOCK_SIZE];

inode_t rootINode;

inode_t* getINode(char* path) {
    dirEntry_t* dirEntry = (dirEntry_t*)SCRATCH_BLOCK_LOC;
    while (dirEntry->name[0] != '\0') {
        if (strcmp(dirEntry->name, path) == 0) break;
        dirEntry++;
    }

    if (dirEntry->name[0] == '\0') return NULL;

    inode_t* inode = (inode_t*)BOOT_FIRST_INODE_LOC;
    while (inode->id != dirEntry->id) {
        inode++;
    }

    return inode;
}

uint8_t loadFile(inode_t* inode, void* dest) {
    uint32_t totalBlocks = bytesToBlocks(inode->sizeBytes);
    uint32_t addressOffset = 0;

    for (uint8_t i = 0; i < 4 && totalBlocks > 0; i++) {
        diskRead(inode->extent[i].firstBlock * 8, inode->extent[i].length * 8, dest + addressOffset);
        addressOffset += inode->extent[i].length * FS_BLOCK_SIZE;
        totalBlocks -= inode->extent[i].length;
    }

    if (totalBlocks > 0) {
        // TODO: Indirect extents
    }

    return 0;
}

#endif
