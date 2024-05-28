#ifndef FILESYS_H
#define FILESYS_H

#include <disk/disk.h>
#include <memory/addresses.h>
#include <string.h>

typedef struct {
    char filename[10];
    char filetype[3];
    uint8_t reserved;
    uint8_t startAddress;
    uint8_t length;
} __attribute__ ((packed)) fileEntry_t;

char* filetable = (char*)FILETABLE_LOC; // Pretty sure we don't have to care about the bootloader at 0x7c00

fileEntry_t* findFile(const char* filename) {
    if (strlen(filename) > 14) { return NULL; } // Filename too long
    static char filenameBuf[16];
    strcpy(filenameBuf, filename);

    char* filetype = strchr(filenameBuf, '.');
    *filetype++ = '\0'; // Split filename and filetype by a NULL

    char* ft = filetable;
    while (*ft != '\0') {
        if (strcmp(filenameBuf, ft) != 0) { // Filename doesn't match
            ft += 16; // Goto next entry
            continue;
        }
        ft += 10; // Goto filetype
        
        if (strcmp(filetype, ft) != 0) {
            ft += 6; // Goto next entry
            continue;
        }
        ft -= 10; // Go back to start of entry
        
        return (fileEntry_t*)ft;
    }

    return NULL; // No such file
}

void updateFiletable() {
    // TODO: Update the filetable by writing data from memory to disk!
}

uint8_t readFile(char* filename, void* buffer) { // Hopefully the buffer is large enought :/
    fileEntry_t* file = findFile(filename);
    if (file == NULL) { return 1; } // File doesn't exist!

    diskRead(file->startAddress, file->length, buffer);
    return 0; // Success
}

uint8_t writeFile(char* filename, void* buffer) { // TODO: Add length so we don't write too much/too little
    fileEntry_t* file = findFile(filetable);
    if (file == NULL) { // File exists -> override
        // File doesn't exist -> create new file
        file = (fileEntry_t*)filetable;
        while (*(char*)file != '\0') { file++; } // Find end of the filetable
         
        *strchr(filename, '.') = '\0';

        char* filetype = filename + strlen(filename); // + 1?
        fileEntry_t* previous = file - 1;

        strcpy(file->filename, filename);
        strcpy(file->filetype, filetype);
        file->startAddress = previous->startAddress + previous->length;
        file->length = 1; // TODO: Add lenght! Currently will just assume 1 sector :/
    }

    diskWrite(file->startAddress, file->length, buffer); // Write the file
    return 0; // Success
}

#endif
