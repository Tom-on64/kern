#include <fs/kernfs.h>
#include <memory/addresses.h>
#include <drivers/disk.h>
#include <fs/fs.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

superblock_t* impl_superblock = (superblock_t*)SUPERBLOCK_LOC;
fsInfo_t* fsInfo = (fsInfo_t*)FS_INFO_LOC;

inode_t* getInode(char* path, dirEntry_t* dentries) {
    while (dentries->name[0] != '\0') {
        if (strcmp(dentries->name, path) == 0) break;
        dentries++;
    }
    if (dentries->name[0] == '\0') { return NULL; }

    inode_t* inode = (inode_t*)BOOT_FIRST_INODE_LOC + dentries->id;

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

void initOpenFileTable() {
    fsInfo->openFileTable = malloc(sizeof(FILE) * MAX_OPEN_FILES);
    if (fsInfo->openFileTable == NULL) { printf("\e[1MFailed to init file table! Malloc() error.\e[8M"); }
    memset(fsInfo->openFileTable, 0, sizeof(FILE) * MAX_OPEN_FILES);

    // offset and ptr are unused in these
    stdin = fsInfo->openFileTable + 0;
    stdin->_file = STDIN_FILENO;
    stdin->_flag = O_RDONLY;

    stdout = fsInfo->openFileTable + 1;
    stdout->_file = STDOUT_FILENO;
    stdout->_flag = O_WRONLY | O_APPEND;

    stderr = fsInfo->openFileTable + 2;
    stderr->_file = STDERR_FILENO;
    stderr->_flag = O_WRONLY | O_APPEND;

    fsInfo->openFiles = 3;
}

