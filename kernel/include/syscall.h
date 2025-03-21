#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <isr.h>

int syscallHandler(struct isr_intFrame* iframe) { return 1; }

#endif
