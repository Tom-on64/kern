#include <kernel.h>
#include <tty.h>

__noreturn
void kmain(void) {
	tty_init();
	printk("Hello, World!\n");
}

