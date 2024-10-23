#include <syscalls/syscalls.h>
#include <interrupt/isr.h>
#include <error/debug.h>

int sys_exit(intFrame_t* iframe) {
    debugf("Tried to exit with %d", iframe->eax);
    return iframe->eax;
}

