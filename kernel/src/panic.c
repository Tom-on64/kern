#include <vsnprintf.h>
#include <kernel.h>
#include <stdarg.h>
#include <system.h>

static int crashing = 0;

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

	pr_emerg("--- [ Kernel panic: %s ] ---\n", buf);	
	//pr_emerg("--- [ End kernel panic: %s ] ---\n", buf);

	// We can add some kind of LED blink or smth later :)
	for (;;) __asm__ volatile ("hlt");
}

