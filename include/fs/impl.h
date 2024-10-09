#ifndef FS_IMPL_H
#define FS_IMPL_H

#include <fs/fs.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_OPEN_FILES 256

typedef struct {
    FILE* openFileTable;
    uint32_t openFiles;
} fsInfo_t;

extern superblock_t* impl_superblock;
extern fsInfo_t* fsInfo;

inode_t* getInode(char* path, dirEntry_t* dentries);
uint8_t loadFile(inode_t* inode, void* dest);

#endif
