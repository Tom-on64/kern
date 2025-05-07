#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <kernel.h>
#include <ipc.h>
#include <isr.h>

#define SYSCALL_MAX	16
enum {
	SYS_SEND,
	SYS_RECV,
};

int syscall_init(void);
void syscall_register(size_t idx, void* handler);
void syscall_handler(struct isr_int_frame* iframe);

// Syscall handlers
int sys_send(pid_t dst, ipc_msg_t* msg);
int sys_recv(pid_t src, ipc_msg_t* msg);

#endif
