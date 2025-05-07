#include <kmalloc.h>
#include <kernel.h>
#include <paging.h>
#include <string.h>
#include <system.h>
#include <sched.h>
#include <gdt.h>
#include <isr.h>
#include <pmm.h>

#include <task.h>

struct task* task_first;
struct task* task_dummy;
struct task* task_current;

int task_init(void) {
	task_first = kzalloc(sizeof(*task_first));
	task_dummy = kzalloc(sizeof(*task_dummy));
	if (task_first == NULL || task_dummy == NULL) return 1;


	// The first task is what's running right now @~@
	task_current = task_first;
	task_current->id = TASK_KERNEL;
	task_current->state = TASK_STATE_READY;
	task_current->pagedir = pag_get_pagedir();
	task_current->kernel_task = 1;
	
	debugf("[task] Multitasking initiated.\n");

	return 0;
}

void task_create(size_t id, uint32_t eip, int kernel_task, uint32_t* pagedir, int argc, char** argv) {
	cli();

	uint32_t kstack = (uint32_t)kmalloc(0x1000 - 16) + (0x1000 - 16);
	uint8_t* kesp = (uint8_t*)kstack;

	kesp -= sizeof(struct isr_int_frame);
	struct isr_int_frame* trap = (struct isr_int_frame*)kesp;
	memset(trap, 0, sizeof(*trap));

	uint16_t codesel = kernel_task ? GDT_KERNEL_CODE : GDT_USER_CODE;
	uint16_t datasel = kernel_task ? GDT_KERNEL_DATA : GDT_USER_DATA;

	trap->cs = codesel;
	trap->ds = datasel;
	trap->es = datasel;
	trap->fs = datasel;
	trap->gs = datasel;

	trap->user_ss = datasel;
	trap->user_esp = USER_STACK;

	trap->eflags = 0x200; // Enable interrupts
	trap->eip = eip;

	kesp -= sizeof(struct task_switch_ctx);
	struct task_switch_ctx* ctx = (struct task_switch_ctx*)kesp;
	ctx->edi = 0;
	ctx->esi = 0;
	ctx->ebx = 0;
	ctx->ebp = 0;

	ctx->eip = (uint32_t)isr_return;

	struct task* current = task_first;
	while (current != NULL) {
		if (current->next == NULL) break;
		current = current->next;
	}

	struct task* new = kzalloc(sizeof(*new));
	current->next = new;

	new->kstack_bottom = kstack;
	new->kstack_pointer = (size_t)kesp;
	new->id = id;
	new->kernel_task = kernel_task;
	new->state = TASK_STATE_READY;
	new->pagedir = pagedir;
	new->heap_start = USER_HEAP;
	new->heap_end = USER_HEAP;

	// We don't need to setup a stack frame
	if (kernel_task) {
		sti();
		return;
	}

	void* old_pagedir = pag_get_pagedir();
	pag_set_pagedir(new->pagedir);

	// Setup argc, argv
	size_t arg_len = 0;
	for (int i = 0; i < argc; i++) arg_len += strlen(argv[i]) + 1;
	size_t page_count = dceil(4 + (argc * 4) + 10 + arg_len, PAGE_SIZE);
 
	for (size_t i = 0; i < page_count; i++) {
		uint32_t paddr = pmm_alloc();
		if (paddr == 0) { // Not enough memory
			// TODO: Do pmm_free() for each page
			kfree(new);
			kfree((void*)kstack);
			pag_set_pagedir(old_pagedir);
			return;
		}
		pag_map(USER_STACK + i * PAGE_SIZE, (uint32_t)paddr, 0);
	}

	int* argc_ptr = (int*)USER_STACK;
	char** argv_ptr = (char**)(USER_STACK + 4);
	*argc_ptr = argc;

	size_t cur = 0;
	for (int i = 0; i < argc; i++) {
		size_t len = strlen(argv[i]);
		char* ptr = (char*)((size_t)argv_ptr + argc * 4 + 4 + cur);

		memcpy(ptr, argv[i], len);
		ptr[len] = '\0';

		argv_ptr[i] = ptr;
		cur += len + 1;
	}

	pag_set_pagedir(old_pagedir);
}

void task_kill(size_t id) {
	cli();

	struct task* current = task_first;
	while (current != NULL) {
		if (current->next && current->next->id == id) break;
		current = current->next;
	}

	struct task* task = current->next;
	if (task == NULL || task->state == TASK_STATE_DEAD) return;
	current->next = task->next;

	void* kstack = (void*)(task->kstack_bottom + 0x1000 - 16);
	kfree(kstack);

	uint32_t* old_pagedir = pag_get_pagedir();
	pag_set_pagedir(task->pagedir);
	size_t heap_start = dceil(task->heap_start, PAGE_SIZE);
	size_t heap_end = dceil(task->heap_end, PAGE_SIZE);

	if (heap_end > heap_start) {
		size_t num = heap_end - heap_start;

		for (size_t i = 0; i < num; i++) {
			uint32_t vaddr = heap_start * PAGE_SIZE + i * PAGE_SIZE;
			uint32_t paddr = pag_virt_to_phys(vaddr);
			pmm_free(paddr);
		}
	}

	pag_set_pagedir(old_pagedir);
	pag_free_pagedir(task->pagedir);
	
	// Workaround so we don't free a task we're in
	if (task_current == task) {
		memset(task_dummy, 0, sizeof(*task_dummy));
		task_current = task_dummy;
	}
	kfree(task);

	schedule();
	sti();
}

void task_set_user_heap(struct task* task, size_t heap_end) {
	if (heap_end <= task->heap_start) {
		debugf("[task] Task %d tried to shrink heap bellow 0.\n", task->id);
		task_kill(task->id);
		return;
	}

	int old_page_top = dceil(task->heap_end, PAGE_SIZE);
	int new_page_top = dceil(heap_end, PAGE_SIZE);

	if (new_page_top == old_page_top) return;
	else if (new_page_top > old_page_top) { // Grow
		int num = new_page_top - old_page_top;

		for (int i = 0; i < num; i++) {
			uint32_t paddr = (uint32_t)pmm_alloc();
			if (paddr == 0) {
				debugf("[task] Task %d tried to grow heap too much.\n", task->id);
				task_kill(task->id);
			}
			uint32_t vaddr = old_page_top * PAGE_SIZE + i * PAGE_SIZE;
			pag_map(vaddr, paddr, PAGE_FLAG_USER);
			memset((void*)vaddr, 0, PAGE_SIZE);
		}
	} else { // Shrink
		int num = old_page_top - new_page_top;

		for (int i = 0; i < num; i++) {
			uint32_t vaddr = old_page_top * PAGE_SIZE + i * PAGE_SIZE;
			uint32_t paddr = pag_virt_to_phys(vaddr);
			pmm_free(paddr);
			pag_umap(vaddr);
		}
	}

	task->heap_end = heap_end;
}

struct task* task_get(size_t id) {
	struct task* find = task_first;
	while (find != NULL) {
		if (find->id == id) break;
		find = find->next;
	}

	return find;
}

uint8_t task_get_state(size_t id) {
	struct task* t = task_get(id);
	if (t == NULL) return TASK_STATE_DEAD;
	return t->state;
}

void task_set_state(size_t id, uint8_t state) {
	struct task* t = task_get(id);
	if (t == NULL) return;
	t->state = state;
}

size_t task_create_id(void) {
	struct task* current = task_first;

	size_t max = current->id;
	current = current->next;
	while (current != NULL) {
		if (max < current->id) max = current->id;
		current = current->next;
	}

	return max + 1;
}

