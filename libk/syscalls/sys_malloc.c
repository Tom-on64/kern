#include <syscalls/syscalls.h>
#include <interrupt/isr.h>
#include <memory/malloc.h>
#include <stddef.h>

int sys_malloc(intFrame_t* iframe) {
    size_t count = iframe->ebx;
    
    // First Malloc()
    if (!mallocListHead) {
        mallocInit(count); // Setup malloc linked list
    }

    void* ptr = mallocNextNode(count);
    mallocMergeFree();

    return (size_t)ptr;
}
