#ifndef _SYSCALLS_SYSCALLS_H
#define _SYSCALLS_SYSCALLS_H

#include <cpu/isr.h>

int sys_exit    (intFrame_t* iframe);
int sys_read    (intFrame_t* iframe);
int sys_write   (intFrame_t* iframe);
int sys_open    (intFrame_t* iframe);
int sys_close   (intFrame_t* iframe);
int sys_seek    (intFrame_t* iframe);
int sys_malloc  (intFrame_t* iframe);
int sys_free    (intFrame_t* iframe);
int sys_sleep   (intFrame_t* iframe);

// Main syscall handler/dispatcher
int syscallHandler(intFrame_t* iframe);

#endif
