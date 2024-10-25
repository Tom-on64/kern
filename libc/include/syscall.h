#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <stddef.h>

#define SYSCALL_COUNT 9

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

inline int syscall(int nr, int a0, int a1, int a2, int a3, int a4) {
    int ret;
    __asm__ volatile (
        "movl %1, %%eax\n"  // Syscall number
        "movl %2, %%ebx\n"  // Arg 0
        "movl %3, %%ecx\n"  // Arg 1
        "movl %4, %%edx\n"  // Arg 2
        "movl %5, %%esi\n"  // Arg 3
        "movl %6, %%edi\n"  // Arg 4
        "int $0x80\n"       // Trigger syscall
        "movl %%eax, %0"    // Return
        : "=r"(ret)         // Output
        : "r"(nr), "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4)  // Input
    );
    return ret;
}

/* Syscall wrappers */
inline int _exit(int status) { return syscall(SYS_EXIT, status, 0, 0, 0, 0); }
inline int read(int fd, void* ptr, size_t count) { return syscall(SYS_READ, fd, (int)ptr, (int)count, 0, 0); }
inline int write(int fd, const void* ptr, size_t count) { return syscall(SYS_WRITE, fd, (int)ptr, (int)count, 0, 0); }
inline int open(const char* path, int oflag) { return syscall(SYS_OPEN, (int)path, oflag, 0, 0, 0); }
inline int close(int fd) { return syscall(SYS_CLOSE, fd, 0, 0, 0, 0); }
inline int seek(int fd, size_t offset, int whence) { return syscall(SYS_SEEK, fd, offset, whence, 0, 0); }
inline int _malloc(int size) { return syscall(SYS_MALLOC, size, 0, 0, 0, 0); }
inline int _free(void* ptr) { return syscall(SYS_FREE, (int)ptr, 0, 0, 0, 0); }
inline int _sleep(int t) { return syscall(SYS_SLEEP, t, 0, 0, 0, 0); }

#endif
