#include <syscalls/syscalls.h>
#include <interrupt/isr.h>
#include <stdio.h>

int sys_read(intFrame_t* iframe) {
    char* s = (char*)iframe->ebx;
    reads(s);

    return 0;
}

