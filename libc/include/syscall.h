#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <stddef.h>

#define MAX_SYSCALLS 9

typedef enum {
    SYS_EXIT,
    SYS_READ,
    SYS_WRITE,
    SYS_OPEN,
    SYS_CLOSE,
    SYS_SEEK,
    SYS_MALLOC,
    SYS_FREE,
    SYS_SLEEP,
} SYSCALL_NUMBERS;

typedef enum {
    O_CREAT     = 0x01,  // Create if does not exist
    O_RDONLY    = 0x02,  // Read only
    O_WRONLY    = 0x04,  // Write only
    O_RDWR      = 0x08,  // Read and write
    O_APPEND    = 0x10, // Always writes at the end (appends)
    O_TRUNC     = 0x20, // Truncate file size and position to 0 on Open
    O_BIN       = 0x40, // Read bytes, not text
} OFLAGS;

int syscall(int nr, int arg0, int arg1, int arg2);

/* Syscall wrappers */
int read(int fd, void* ptr, size_t count);
int write(int fd, const void* ptr, size_t count);
int open(const char* path, int oflag);
int close(int fd);

#endif
