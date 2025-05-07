#include <kernel.h>
#include <task.h>
#include <tty.h>

#include <syscall.h>

// Syscall table
void* syscalls[SYSCALL_MAX] = { 0 };

void sys_exit(int code) {
	debugf("[syscall] Task #%d exited with code %d.\n", task_current->id, code);
	task_kill(task_current->id);
}

void sys_test(char* msg) {
	debugf("[syscall] Test: %s\n", msg);
}

int syscall_init(void) {
	syscall_register(SYS_EXIT, (void*)sys_exit);
	syscall_register(SYS_TEST, (void*)sys_test);

	debugf("[syscall] Initiated syscalls!\n");

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
		: "=a"(ret)
		: "g"(iframe->ebp), "g"(iframe->edi), "g"(iframe->esi),
		  "g"(iframe->edx), "g"(iframe->ecx), "g"(iframe->ebx),
		  "g"(handler)
	);
	iframe->eax = ret;
}

