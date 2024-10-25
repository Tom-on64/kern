#ifndef _STDIO_H
#define _STDIO_H

#include <stddef.h>
#include <stdarg.h>

// End Of File
#define EOF (-1)

// File related macros
#define BUFSIZ          512
#define FILENAME_MAX    255
#define FOPEN_MAX       32

// Seek positions
#define SEEK_CUR    0
#define SEEK_END    1
#define SEEK_SET    2

// Standard file pointers
#define STDIN_FILENO    0
#define STDOUT_FILENO   1
#define STDERR_FILENO   2

/* The ultimate FILE struct */
typedef struct _iobuf { // 24B
    unsigned char*  _ptr;       // File buffer base
    unsigned int    _size;      // Size of buffer
    int             _offset;    // Offset into file
    unsigned short  _flag;      // Open flags
    int             _file;      // File descriptor
    
    char            __reserved[6];
} FILE;

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

/* File operations */
FILE* fopen(const char* path, const char* mode);
int fclose(FILE* stream);
size_t fread(void* ptr, size_t size, size_t count, FILE* stream);
size_t fwrite(void* ptr, size_t size, size_t count, FILE* stream);
int fseek(FILE* stream, long offset, int whence);
long ftell(FILE* stream);
void rewind(FILE* stream);
int fflush(FILE* stream);

/* Character I/O */
int getchar(void);
int putchar(int c);
int getc(FILE* stream);
int putc(int c, FILE* stream);
int ungetc(int c, FILE* stream);

/* Line I/O */
char* fgets(char* str, int n, FILE* stream);
int fputs(const char* str, FILE* stream);
int puts(const char* str);

/* Error handling */
void perror(const char* s);
int feof(FILE* stream);
int ferror(FILE* stream);
void clearerr(FILE* stream);

/* Formatted I/O */
int printf(const char* fmt, ...);
int vprintf(const char* fmt, va_list va);
int fprintf(FILE* stream, const char* fmt, ...);
int vfprintf(FILE* stream, const char* fmt, va_list va);
// INFO: (v)sprinf() Has been depricated and removed as it is unsafe (potencial buffer overflow). Please use (v)snprintf()
int snprintf(char* str, size_t size, const char* fmt, ...);
int vsnprintf(char* str, size_t size, const char* fmt, va_list va);

int scanf(const char* fmt, ...);
int vscanf(const char* fmt, va_list va);
int fscanf(FILE* stream, const char* fmt, ...);
int vfscanf(FILE* stream, const char* fmt, va_list va);
int sscanf(const char* str, const char* fmt, ...);
int vsscanf(const char* str, const char* fmt, va_list va);

#endif
