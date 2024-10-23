#include <syscalls/syscalls.h>
#include <interrupt/isr.h>
#include <memory/malloc.h>

int sys_free(intFrame_t* iframe) {
    void* ptr = (void*)iframe->ebx;
    mallocFree(ptr);
    return 0;
}

