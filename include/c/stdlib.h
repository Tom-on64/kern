#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>

void* malloc(const uint32_t size) { // Use Syscall(3) - Malloc()
    void* ptr = 0;
    __asm__ volatile ("int $0x80" : : "a"(3), "b"(size));
    __asm__ volatile ("movl %%eax, %0" : "=r"(ptr));

    return ptr;
}

void free(const void* ptr) { // Use Syscall(4) - Free()
    __asm__ volatile ("int $0x80" : : "a"(4), "b"(ptr));
}

#endif
