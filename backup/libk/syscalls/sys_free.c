#include <syscalls/syscalls.h>
#include <memory/malloc.h>
#include <cpu/isr.h>

int sys_free(intFrame_t* iframe) {
    void* ptr = (void*)iframe->ebx;
    mallocFree(ptr);
    return 0;
}

