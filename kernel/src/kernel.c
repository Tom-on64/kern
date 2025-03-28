#include <bootloader.h>
#include <kernel.h>
#include <paging.h>
#include <serial.h>
#include <gdt.h>
#include <isr.h>
#include <pmm.h>
#include <tty.h>

// TODO: Commented out stuff

__noreturn
void kmain(void* ptr, uint32_t magic) {
	if (boot_initMB1(ptr, magic) != 0) panic("Unsupporred bootloader.");

	// Serial console for debugging
	serial_init(COM1);
	// ESC[H moves cursor to (0,0), ESC[J clears from cursor to end of screen
	// ESC[<x>m sets graphics mode, 0 - reset, 36 - cyan
	debugf("\x1b[H\x1b[J\x1b[0mkern. \x1b[36m(serial console)\x1b[0m\n\n");

	// VGA tty thingy
	tty_init();
	tty_puts("kern.\n\n");

	// Memory manager init
	// pmm_init();
	// vmm_init();

	gdt_init();
	isr_init();
	pag_init();

	debugf("Basic initialization complete!\n");
	
	panic("kmain() reached end.");

	// Temporary loop since panic() does not halt
	while (1);
}

