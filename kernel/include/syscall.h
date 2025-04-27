#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <isr.h>

int syscall_handler(struct isr_int_frame* iframe) { (void)iframe; return 1; }

#endif
