#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>
#include <syscall.h>

void* malloc(const uint32_t size) { 
    void* ptr = 0;
    __asm__ volatile ("int $0x80" : : "a"(SYS_MALLOC), "b"(size));
    __asm__ volatile ("movl %%eax, %0" : "=r"(ptr));

    return ptr;
}

void free(const void* ptr) { 
    __asm__ volatile ("int $0x80" : : "a"(SYS_FREE), "b"(ptr));
}

#endif
