#include <syscalls/syscalls.h>
#include <screen/tty.h>
#include <cpu/isr.h>
#include <stddef.h>

int sys_write(intFrame_t* iframe) {
    int fd = iframe->ebx;
    void* buf = (void*)iframe->ecx;
    size_t len = iframe->edx;

    if (fd == 1) {
        return consoleWrite(buf, len);
    } else { return -1; }
}

