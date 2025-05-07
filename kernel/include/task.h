#ifndef _TASK_H
#define _TASK_H

#include <kernel.h>

#define TASK_KERNEL 0	// Main kernel task ID

enum {
	TASK_STATE_DEAD,
	TASK_STATE_IDLE,
	TASK_STATE_READY,
	TASK_STATE_RECV,
};

/*
 * Each task has an ID. This is used as a reference to the task.
 * A task has it's own paging setup, so it must have it's own pagedir.
 * Tasks have state. These states are defined as follows:
 * 	TASK_STATE_DEAD - This task has been killed
 *	TASK_STATE_READY - This task is ready for scheduling
 *	TASK_STATE_IDLE - This task doesn't need processor time
 * Each task has it's own kernel stack to push registers and such to when
 * context switching. To switch to a task, you load kstack_pointer into ESP and
 * pop the state.
 * The kstack_bottom is loaded into ESP using the TSS when switching from user
 * to kernel space on interrupts. 
 * Tasks are stored in a linked list, so it must have a reference to the next 
 * task.
 *
 * WARN: Be careful when modifing this struct, it's contents are accessed from 
 * 	 task.s which uses hardcoded offsets.
 */
struct task {
	pid_t id;

	size_t kstack_pointer;
	size_t kstack_bottom;

	uint32_t* pagedir;
	uint8_t	state;
	int kernel_task;

	size_t heap_start;
	size_t heap_end;

	struct task* next;
};

// Describes contents of the kstack when task switching
struct task_switch_ctx {
	uint32_t edi;
	uint32_t esi;
	uint32_t ebx;
	uint32_t ebp;
	uint32_t eip;
};

extern struct task* task_first;
extern struct task* task_current;

int task_init(void);
void task_create(pid_t id, uint32_t eip, int kernel_task, uint32_t* pagedir, int argc, char** argv);
void task_kill(pid_t id);
void task_set_user_heap(pid_t id, size_t heap_end);
struct task* task_get(pid_t id);
uint8_t task_get_state(pid_t id);
void task_set_state(pid_t id, uint8_t state);
pid_t task_create_id(void);

// Defined in task.s
extern void task_switch(struct task* old, struct task* new);

#endif
