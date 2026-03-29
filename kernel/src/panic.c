#include <vsnprintf.h>
#include <kernel.h>
#include <stdarg.h>
#include <system.h>
#include <isr.h>
#include <pmm.h>

static int crashing = 0;

void panic_regs_dump(struct isr_int_frame* regs) {
	if (regs == NULL) return;

	pr_emerg("Register Dump:\n");
	pr_emerg("\tRAX: %016lx RBX: %016lx RCX: %016lx\n", regs->rax, regs->rbx, regs->rcx);
	pr_emerg("\tRDX: %016lx RSI: %016lx RDI: %016lx\n", regs->rdx, regs->rsi, regs->rdi);
	pr_emerg("\tR08: %016lx R09: %016lx R10: %016lx\n", regs->r8, regs->r9, regs->r10);
	pr_emerg("\tR11: %016lx R12: %016lx R13: %016lx\n", regs->r11, regs->r12, regs->r13);
	pr_emerg("\tR14: %016lx R15: %016lx DS/ES: %016lx\n", regs->r14, regs->r15, regs->ds);
	pr_emerg("\tRBP: %016lx RSP: %016lx SS: %016lx\n", regs->rbp, regs->user_rsp, regs->user_ss);
	pr_emerg("\tRIP: %016lx RFLAGS: %016lx CS: %016lx\n", regs->rip, regs->rflags, regs->cs);
	pr_emerg("\n");
}

void panic_call_trace(void) {
	uintptr_t* rbp;
	__asm__ volatile ("movq %%rbp, %0" : "=r"(rbp));

	pr_emerg("Call trace:\n");
	for (size_t i = 0; i < 16 && (uintptr_t)rbp > 0x1000; i++) {
		uintptr_t retaddr = rbp[1];
		offset_t offset = 0;

		// TODO: Parse ELF and print function names
		char* fname = "???";

		pr_emerg("  [<%p>] %s+0x%lx\n", (void*)retaddr, fname, offset);

		rbp = (uintptr_t*)*rbp;
	}
	pr_emerg("\n");
}

void panic(struct isr_int_frame* regs, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vpanic(regs, fmt, ap);
	va_end(ap);
}

void vpanic(struct isr_int_frame* regs, const char *fmt, va_list ap) {
	static char buf[PANIC_MSG_BUFSZ];
	size_t len;

	cli();

	if (crashing) { for (;;) __asm__ volatile ("hlt"); }
	crashing = true;

	len = vsnprintf(buf, sizeof(buf), fmt, ap);
	if (len && buf[len - 1] == '\n') buf[len - 1] = '\0';

	pr_emerg("--- [ Kernel panic: %s ] ---\n\n", buf);	
	pr_emerg("%lu / %lu pages free.\n\n", pmm_free_pages, pmm_total_pages);
	panic_regs_dump(regs);
	panic_call_trace();
	pr_emerg("--- [ End kernel panic: %s ] ---\n", buf);

	// We can add some kind of LED blink or smth later :)
	for (;;) __asm__ volatile ("hlt");
}

