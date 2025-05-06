#include <kernel.h>
#include <task.h>
#include <gdt.h>

#include <sched.h>

void schedule(void) {
	if (task_first == NULL) return;

	struct task* next = task_current->next;
	if (next == NULL) next = task_first;

	while (next->state != TASK_STATE_READY) {
		next = next->next;
		if (next == NULL) next = task_first;
	}

	struct task* old = task_current;
	task_current = next;

	tss_esp0(next->kstack_bottom);

	// NOTE: Might not return
	task_switch(old, next);
}

