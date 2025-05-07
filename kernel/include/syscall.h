#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <kernel.h>
#include <isr.h>

#define SYSCALL_MAX	2
#define SYS_EXIT	0
#define SYS_TEST	1

int syscall_init(void);
void syscall_register(size_t idx, void* handler);
void syscall_handler(struct isr_int_frame* iframe);

#endif
