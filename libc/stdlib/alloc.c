#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <syscall.h>

void* malloc(size_t size) {
    return (void*)syscall(SYS_MALLOC, size, 0, 0);
}

void* calloc(size_t count, size_t size) {
    size_t total = count * size;
    void* ptr = malloc(total);
    
    if (!ptr) { return NULL; }
    memset(ptr, 0, total);

    return ptr;
}

void* realloc(void* ptr, size_t size) {
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    if (ptr == NULL) { return malloc(size); }

    void* newptr = malloc(size);
    if (!newptr) { return NULL; }

    // TODO: retrieve the old size
    memcpy(newptr, ptr, size);

    free(ptr);
    return newptr;
}

void free(void* ptr) {
    syscall(SYS_FREE, (int)ptr, 0, 0);
}

