#include <syscall.h>
#include <stddef.h>

#define MAX_SYSCALLS 9

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

