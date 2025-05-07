#include <kernel.h>

#include <ipc.h>

int ipc_send(pid_t dst, ipc_msg_t* msg) {
	(void)dst;
	(void)msg;
	return 0;
}

int ipc_recv(pid_t src, ipc_msg_t* msg) {
	(void)src;
	(void)msg;
	return 0;
}

