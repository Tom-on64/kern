#ifndef STDIO_H
#define STDIO_H

#include <syscall.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

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

/* Declarations */
int printf(const char*, ...);

/* --- file I/O --- */
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

/* --- stdout --- */
void putc(char c) {
    // write(stdout, &c, 1); TODO: This doesn't work and we need to use a static variable
    static char _c;
    _c = c;
    write(1, &_c, 1);
}

void print(const char* s) {
    write(1, s, strlen(s));
}

void clear() {
    print("\x1b[J");
}

// TODO: Support negative numbers
int printf(const char* fmt, ...) {
    unsigned int* argPtr = (unsigned int*)&fmt + 1; // First arg after fmt
    
    // Printing buffer
    static char* buffer;
    static unsigned int len;
    int max = 256;

    buffer = malloc(max);
    len = 0;
    
    int state = 0;
    for (int i = 0; fmt[i] != '\0'; i++) {
        if (len > max) {
            max *= 2;
            // Reallocate larger buffer TODO: Use realloc();
            char* temp = malloc(max);
            strcpy(temp, buffer);
            free(buffer);
            buffer = temp;
        }
        buffer[len] = '\0';

        char c = fmt[i];
        if (state == 0) {
             if (c == '%') state = '%';
             else buffer[len++] = c;
        } else if (state == '%') {
            switch (c) {
                case 'd': // Decimal number
                    strcat(buffer, itoa(*(unsigned int*)argPtr++, 10));
                    len += strlen(&buffer[len]); // Add the remaining length
                    break;
                case 'x': // Hex number
                    strcat(buffer, itoa(*(unsigned int*)argPtr++, 16));
                    len += strlen(&buffer[len]);
                    break;
                case 's': // String
                    char* s = *(char**)argPtr++;
                    
                    while (*s != '\0') {
                        buffer[len++] = *s++;
                    }
                    break;
                case 'c': // Char
                    buffer[len++] = *(char*)argPtr;
                    argPtr++;
                    break;
                default: // This looks insane
                    buffer[len++] = '%';
                case '%': // Just print '%'
                    buffer[len++] = c;
                    break;
            }  
            state = 0;
        }
    }

    buffer[len] = '\0';
    write(1, buffer, len);
    free(buffer);

    return 0;
}

/* --- stdin --- */
char getc() {
    char* character = (char*)0x2100;
    *character = '\0';
    while (*character == '\0') { __asm__ volatile ("hlt"); }
    return *character;
}

// Reads until a newline
// TODO: Use Read() Syscall
void reads(char* buffer) {
    char c;
    int len = 0;

    while ((c = getc()) != '\n') {
        if (c == '\b') {
            if (len < 1) continue; // Can we backspace?
            len--;
            putc(c);
        } else {
            buffer[len++] = c;
            putc(c);
        }
    }

    putc(c); // Print the newline
    buffer[len] = '\0'; // Null terminate
}

#endif
