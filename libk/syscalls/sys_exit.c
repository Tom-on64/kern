#include <syscalls/syscalls.h>
#include <drivers/serial.h>
#include <cpu/isr.h>

int sys_exit(intFrame_t* iframe) {
    debugf("Tried to exit with %d", iframe->eax);
    return iframe->eax;
}

