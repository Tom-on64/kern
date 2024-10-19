#include <stdint.h>
#include <syscall.h>
#include <stdlib.h>

void* malloc(const uint32_t size) { 
    return (void*)syscall(SYS_MALLOC, size, 0, 0);
}

void free(const void* ptr) { 
    syscall(SYS_FREE, (int)ptr, 0, 0);
}

