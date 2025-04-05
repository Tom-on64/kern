#include <bootloader.h>
#include <kernel.h>
#include <paging.h>
#include <serial.h>
#include <gdt.h>
#include <isr.h>
#include <pmm.h>
#include <tty.h>

__noreturn
void kmain(void* ptr, uint32_t magic) {
	if (boot_initMB1(ptr, magic) != 0) panic("Unsupporred bootloader.");

	// Serial console for debugging
	if (serial_init(COM1) != 0) panic("Failed to initialize Serial driver.");
	debugf("\x1b[H\x1b[J\x1b[0mkern. \x1b[36m(serial console)\x1b[0m\n\n");

	// VGA tty thingy
	if (tty_init() != 0) panic("Failed to initalize TTY driver.");
	tty_puts("kern.\n\n");

	// System init
	if (gdt_init() != 0) panic("Failed to initalize GDT.");
	if (isr_init() != 0) panic("Failed to initalize ISRs.");
	if (pag_init() != 0) panic("Failed to initalize Paging.");

	// Memory manager init
	if (pmm_init() != 0) panic("Failed to initalize Physical Memory Manager.");
	//if (vmm_init() != 0) panic("Failed to initalize Virtual Memory Manager.");

	debugf("Basic initialization complete!\n");
	
	panic("kmain() reached end.");
}

