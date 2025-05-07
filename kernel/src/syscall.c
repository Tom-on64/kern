#include <kernel.h>
#include <task.h>
#include <ipc.h>
#include <tty.h>

#include <syscall.h>

// Syscall table
void* syscalls[SYSCALL_MAX] = { 0 };

int syscall_init(void) {
	syscall_register(SYS_SEND, (void*)sys_send);
	syscall_register(SYS_RECV, (void*)sys_recv);

	debugf("[syscall] Initiated syscalls!\n");

	return 0;
}

int sys_send(pid_t dst, ipc_msg_t* msg) {
	msg->sender = task_current->id;
	debugf("[syscall] Task #%d sent a message to Task #%d.\n", msg->sender, dst);
	return ipc_send(dst, msg);
}

int sys_recv(pid_t src, ipc_msg_t* msg) {
	debugf("[syscall] Task #%d is receiving a message from Task #%d.\n", task_current->id, src);
	return 0;
}

void syscall_register(size_t idx, void* handler) {
	if (idx > SYSCALL_MAX) {
		debugf("[syscall] Failed to register syscall #%d.\n", idx);
		return;
	}

	syscalls[idx] = handler;
}

void syscall_handler(struct isr_int_frame* iframe) {
	size_t nr = iframe->eax;
	if (nr > SYSCALL_MAX || !syscalls[nr]) {
		iframe->eax = -1;
		debugf("[syscall] Tried to call syscall #%d which doesn't exist.\n", nr);
		return;
	}

	void* handler = syscalls[nr];

	int ret;
	debugf("0x%08x\n", &ret);
	__asm__ volatile(
		"push %1\n"
		"push %2\n"
		"push %3\n"
		"push %4\n"
		"push %5\n"
		"push %6\n"
		"call *%7\n"
		"add $24, %%esp\n"
		"mov %%eax, %0\n"
		: "=r"(ret)
		: "g"(iframe->ebp), "g"(iframe->edi), "g"(iframe->esi),
		  "g"(iframe->edx), "g"(iframe->ecx), "g"(iframe->ebx),
		  "g"(handler)
		: "eax", "memory"
	);
	iframe->eax = ret;
}

