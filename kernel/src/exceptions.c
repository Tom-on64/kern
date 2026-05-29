#include "paging.h"
#include <exceptions.h>
#include <kernel.h>
#include <string.h>
#include <errno.h>
#include <isr.h>

exception_handler_ptr exception_handlers[EXCEPTION_COUNT];

const char* exception_strings[] = {
	"Division Error",
	"Debug",
	"Non-maskable Interrupt",
	"Breakpoint",
	"Overflow",
	"Bound Range Exceeded",
	"Invalid Opcode",
	"Device Not Available",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Invalid TSS",
	"Segment Not Present",
	"Stack-Segment Fault",
	"General Protection Fault",
	"Page Fault",
	"Reserved",
	"x87 Floating-Point Exception",
	"Alignment Check",
	"Machine Check",
	"SIMD Floating-Point Exception",
	"Virtualization Exception",
	"Control Protection Exception",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Hypervisor Injection Exception",
	"VMM Communication Exception",
	"Security Exception",
	"Reserved",
};

void exception_handle(struct isr_int_frame* iframe) {
	bool user = (iframe->cs & 3) == 3;

	if (exception_handlers[iframe->interrupt]) {
		exception_handlers[iframe->interrupt](iframe, user);
	} else {
		panic(iframe, "Unhandled %s exception, err 0x%lx", exception_strings[iframe->interrupt], iframe->error);
	}
}

void handle_division_error(struct isr_int_frame* iframe, bool user) {
	if (user) {
		// TODO: Kill process
	} else panic(iframe, "Division by 0 @ %p", (void*)iframe->rip);

	
}

void handle_invalid_opcode(struct isr_int_frame* iframe, bool user) {
	if (user) {
		pr_error("Userspace #UD @ %p\n", (void*)iframe->rip);
		// TODO: Kill process
	} else {
		panic(iframe, "Kernel #UD");
	}
}

void handle_device_not_available(struct isr_int_frame* iframe, bool user) {
	(void)iframe;
	(void)user;

	__asm__ volatile("clts");

	// TODO:
	// if (last_fpu_user != current) {
	// 	if (last_fpu_user) {
	// 	__asm__ volatile("fxsaveq %0" : "=m"(last_fpu_user->fpu_state));
	// 	}
	// 	__asm__ volatile("fxrstorq %0" : : "m"(current->fpu_state));
	// 	set_last_fpu_owner(current);
	// }

	pr_notice("FPU/SSE initialized lazily for current thread.\n");
}

// Do not attempt to log gracefully using complex structures or locks
// The kernel stack might be completely shredded
void handle_double_fault(struct isr_int_frame* iframe, bool user) {
	(void)user;

	pr_emerg(
		"Beware human. This thing is beyond comprehension. Beyond this corporeal realm.\n"
		"Oh may the those unlucky enough to encouter it be spared of their pain.\n"
		"Through it you pass into the city of woe.\n"
		"Through it you pass into eternal pain.\n"
		"To rear it was the task of power divine.\n"
		"Supremes wisdom, and primeval love.\n"
		"Before it things create were none, save things.\n"
		"Eternal, and eternal It endures.\n"
		"Abandon all hope, all ye who enter here.\n\n"
		);
	panic(iframe, "[   D O U B L E   F A U L T   ]");
}

void handle_general_protection_fault(struct isr_int_frame* iframe, bool user) {
	if (user) {
		pr_error("Userspace #GP @ %p (err: 0x%lx)\n", (void*)iframe->rip, iframe->error);
		// TODO: Kill process
	}
}

void handle_page_fault(struct isr_int_frame* iframe, bool user) {
	uint64_t fault_addr;
	__asm__ volatile("mov %%cr2, %0" : "=r"(fault_addr));

	uint64_t err = iframe->error;
	bool present = err & 0x1;
	bool write   = err & 0x2;
	bool usersp  = err & 0x4;
	bool fetch   = err & 0xA;

	if (user) {
		pr_error("Userspace #PF: Segfault @ %p, tried to access %p\n", (void*)iframe->rip, (void*)fault_addr);
		pr_error("\x1b[9m  Reason: Retard\x1b[m\n"); // reference
		pr_error("  Reason: %s %s in %s mode %s\n",
			present ? "Protection violation" : "Non-present page",
			write   ? "during WRITE" : "during READ",
			usersp  ? "user" : "supervisor",
			fetch   ? "during instruction fetch" : ""
			);
		// TODO: Kill process
	} else {
		panic(iframe, "Kernel: Segmentation fault @ %p, tried to access %p (err: 0x%lx)", (void*)iframe->rip, (void*)fault_addr, err);
	}
}

int exception_init(void) {
	memset(exception_handlers, 0, sizeof(exception_handlers));

	exception_handlers[EXCEPTION_DE] = handle_division_error;
	exception_handlers[EXCEPTION_UD] = handle_invalid_opcode;
	exception_handlers[EXCEPTION_NM] = handle_device_not_available;
	exception_handlers[EXCEPTION_DF] = handle_double_fault;
	exception_handlers[EXCEPTION_PF] = handle_page_fault;

	return SUCCESS;
}

