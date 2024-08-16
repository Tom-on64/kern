#ifndef SYSCALL_H
#define SYSCALL_H

#include <stddef.h>

#define MAX_SYSCALLS 9

typedef enum {
    SYS_RESERVED,
    SYS_SLEEP, 
    SYS_READ,
    SYS_WRITE,
    SYS_OPEN,
    SYS_CLOSE,
    SYS_MALLOC,
    SYS_FREE,
    SYS_SEEK,
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

int syscall(int nr, int arg0, int arg1, int arg2) {
    int ret = 0;
    __asm__ volatile ("int $0x80" : "=a"(ret) : "a"(nr), "b"(arg0), "c"(arg1), "d"(arg2));
    return ret;
}

/* Syscall wrappers */
int read(int fd, void* ptr, size_t count) {
    // Returns: int fd
    return syscall(SYS_READ, fd, (int)ptr, count);
}

int write(int fd, const void* ptr, size_t count) {
    // Returns: int count
    return syscall(SYS_WRITE, fd, (int)ptr, count);
}

int open(const char* path, int oflag) {
    // Returns: int fd
    return syscall(SYS_OPEN, (int)path, oflag, 0);
}

int close(int fd) {
    // Returns: int status
    return syscall(SYS_CLOSE, fd, 0, 0);
}

#endif
