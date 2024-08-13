#ifndef IMPL_H
#define IMPL_H

#include <fs/fs.h>
#include <memory/addresses.h>
#include <disk/disk.h>
#include <stdint.h>

char* currentDir;

inode_t* getInode(char* path) {
    dirEntry_t* dentry = (dirEntry_t*)SCRATCH_BLOCK_LOC;

    while (dentry->name[0] != '\0') {
        if (strcmp(dentry->name, path)) break;
        dentry++;
    }

    if (dentry->name[0] == '\0') return NULL;

    inode_t* inode = (inode_t*)BOOT_FIRST_INODE_LOC;
    while (inode->id != dentry->id) { inode++; }

    return inode;
}

uint8_t loadFile(inode_t* inode, void* dest) {
    uint32_t totalBlocks = BYTES2BLOCKS(inode->sizeBytes);
    uint32_t offset = 0;

    for (uint8_t i = 0; i < 4 && totalBlocks > 0; i++) {
        diskRead(inode->extent[i].block * 8, inode->extent[i].length * 8, dest + offset);
        offset += inode->extent[i].length * FS_BLOCK_SIZE;
        totalBlocks -= inode->extent[i].length;
    }

    if (totalBlocks > 0) {
        // TODO: Indirect extents
        return 1;
    }

    return 0;
}

#endif
