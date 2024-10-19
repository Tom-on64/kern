#ifndef _STDIO_H
#define _STDIO_H

#include <stddef.h>

// Nobody should have to interface with the internals of this structure
typedef struct _iobuf { // 24B
    unsigned char*  _ptr;       // File buffer base
    unsigned int    _size;      // Size of buffer
    int             _offset;    // Offset into file
    unsigned short  _flag;      // Open flags
    int             _file;      // File descriptor
    
    char            __reserved[6];
} FILE;

// Standard file pointers
extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

/* File IO */
FILE* fopen(const char* filename, const char* mode);
int fclose(FILE* stream);
size_t fread(void* ptr, size_t size, size_t count, FILE* stream);
size_t fwrite(const void* ptr, size_t size, size_t count, FILE* stream);
int fseek(FILE* stream, long int origin, int offset);
long int ftell(FILE* stream);

/* Terminal IO */
int printf(const char* fmt, ...);
int sprintf(char* str, const char* fmt, ...);
int snprintf(char* str, size_t n, const char* fmt, ...);
int fprintf(FILE* stream, const char* fmt, ...);
void putc(char c);
char getc();
void print(const char* str);
void clear();
void reads(char* str);

#endif
