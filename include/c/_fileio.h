#ifndef _FILEIO_H
#define _FILEIO_H

#include <stdint.h>
#include <syscall.h>

// Nobody should have to interface with the internals of this structure
typedef struct _iobuf { // 24B
    unsigned char*  _ptr; // File buffer base
    unsigned int    _size; // Size of buffer
    int             _offset; // Offset into file
    unsigned short  _flag; // Open flags
    int             _file; // File descriptor
    
    char            __reserved[6];
} FILE;

FILE* stdin;
FILE* stdout;
FILE* stderr;

FILE* fopen(const char* filename, const char* mode) {
    int oflag = O_RDWR | O_CREAT | O_BIN; // TODO!

    int fd = open(filename, oflag);
    if (fd < 0) { 
        printf(""); // TODO: i'd prefer if i didn't need this
        return NULL; 
    }
   
    // TODO: Somehow get the open file table
    FILE* fp = malloc(sizeof(FILE));
    fp->_file = fd;

    return fp;
}

int fclose(FILE* stream) {
    int res = close(stream->_file);
    return res;
}

size_t fread(void* ptr, size_t size, size_t count, FILE* stream) {
    unsigned int offset = 0;
    size_t readEl = 0;
    int fd = stream->_file;

    while (readEl != count) {
        if (read(fd, (void*)(ptr + offset), size) != size) break;
        offset += size;
        readEl++;
    }

    return readEl;
}

size_t fwrite(const void* ptr, size_t size, size_t count, FILE* stream) {
    unsigned int offset = 0;
    size_t writtenEl = 0;
    int fd = stream->_file;

    while (writtenEl != count) {
        if (write(fd, (void*)(ptr + offset), size) != size) break;
        offset += size;
        writtenEl++;
    }

    return writtenEl;
}

int fseek(FILE* stream, long int origin, int offset) {
    // TODO
    return -1;
}

long int ftell(FILE* stream) {
    // TODO
    return -1;
}

#endif

