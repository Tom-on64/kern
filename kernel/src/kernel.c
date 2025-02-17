#include <kernel.h>
#include <tty.h>

#define _hlt()	while (1) __asm__ volatile ("cli; hlt");

__noreturn
void kmain(void) {
	tty_init();
	tty_puts("Hello, World!\n");

	_hlt();
}

