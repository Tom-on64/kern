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

int syscall(int nr, int arg0, int arg1, int arg2);

/* Syscall wrappers */
int read(int fd, void* ptr, size_t count);
int write(int fd, const void* ptr, size_t count);
int open(const char* path, int oflag);
int close(int fd);

#endif
