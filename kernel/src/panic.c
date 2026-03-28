#include <vsnprintf.h>
#include <kernel.h>
#include <stdarg.h>
#include <system.h>
#include <pmm.h>

static int crashing = 0;

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
}

void panic(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vpanic(fmt, ap);
	va_end(ap);
}

void vpanic(const char *fmt, va_list ap) {
	static char buf[PANIC_MSG_BUFSZ];
	size_t len;

	cli();

	if (crashing) { for (;;) __asm__ volatile ("hlt"); }
	crashing = true;

	len = vsnprintf(buf, sizeof(buf), fmt, ap);
	if (len && buf[len - 1] == '\n') buf[len - 1] = '\0';

	pr_emerg("--- [ Kernel panic: %s ] ---\n\n", buf);	

	pr_emerg("%lu / %lu pages free.\n\n", pmm_free_pages, pmm_total_pages);
	panic_call_trace();

	pr_emerg("\n--- [ End kernel panic: %s ] ---\n", buf);

	// We can add some kind of LED blink or smth later :)
	for (;;) __asm__ volatile ("hlt");
}

