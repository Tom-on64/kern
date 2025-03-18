#include <bootloader.h>
#include <kernel.h>
#include <serial.h>
#include <pmm.h>
#include <tty.h>

#define _hlt()	while (1) __asm__ volatile ("cli; hlt");

__noreturn
void kmain(void* ptr, uint32_t magic) {
	// Init with MultiBoot 1, this populates the global bootloader struct
	//if (boot_init(BOOT_MB1, ptr, magic) != 0) panic("Unsupporred bootloader.");

	// Serial console for debugging
	serial_init(COM1);
	serial_puts(COM1, "\x1b[0H\x1b[Jkern.\n");

	// VGA tty thingy
	tty_init();
	tty_puts("kern.\n\n");

	// Memory manager init
	// pmm_init();
	// vmm_init();

	// gdt_init();
	// isr_init();
	// pag_init();

	debugf("Basic initialization complete!\n");
	
	/* TODO: Commented out stuff */

	panic("kmain() reached end.");
}

