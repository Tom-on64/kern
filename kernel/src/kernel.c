#include <kernel.h>
#include <serial.h>
#include <tty.h>

#define _hlt()	while (1) __asm__ volatile ("cli; hlt");

__noreturn
void kmain(void) {
	serial_init(COM1);
	serial_print(COM1, "kern.\n\n");
	while (1) {
		char c = serial_recv(COM1);
		serial_send(COM1, c);
		if (c == '\r') {
			serial_send(COM1, '\n');
			break;
		}
	}

	tty_init();
	tty_puts("Hello, World!\n");

	panic("kmain() reached end!");
}

