#ifndef SYSCALLNUMS_H
#define SYSCALLNUMS_H

#include <stdint.h>

#define MAX_SYSCALLS 5

typedef enum {
    SYS_SLEEP, 
    SYS_WRITE,
    SYS_GETS,
    SYS_MALLOC,
    SYS_FREE,
} SYSCALL_NUMBERS;

// TODO: Add malloc() and free() syscall wrappers??
int32_t write(int32_t fd, const void* buf, uint32_t len) {
    int32_t result = -1;
    __asm__ volatile ("int $0x80" : "=a"(result) : "a"(SYS_WRITE), "b"(fd), "c"(buf), "d"(len));
    return result;
}

#endif
