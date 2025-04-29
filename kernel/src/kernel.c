#include <bootloader.h>
#include <kmalloc.h>
#include <kernel.h>
#include <paging.h>
#include <serial.h>
#include <timer.h>
#include <gdt.h>
#include <isr.h>
#include <pmm.h>
#include <tty.h>

__noreturn
void kmain(void* ptr, uint32_t magic) {
	if (boot_init_mb1(ptr, magic) != 0) panic("Unsupported bootloader.");

	// Serial console for debugging
	if (serial_init(COM1) != 0) panic("Failed to initialize Serial driver.");
	debugf("\x1b[H\x1b[J\x1b[0mkern. \x1b[36m(serial console)\x1b[0m\n\n");

	// System init
	if (gdt_init() != 0) panic("Failed to initalize GDT.");
	if (isr_init() != 0) panic("Failed to initalize ISRs.");
	if (pag_init() != 0) panic("Failed to initalize Paging.");

	// Memory manager init
	if (pmm_init() != 0) panic("Failed to initalize Physical Memory Manager.");
	if (kmalloc_init() != 0) panic("Failed to initialize kernel heap.");

	// VGA TTY
	if (tty_init() != 0) panic("Failed to initalize TTY driver.");
	tty_puts("kern.\n\n");

	if (timer_init() != 0) panic("Failed to initialize timer.");

	// TODO: Scheduler
	// TODO: Syscalls
	// TODO: Interprocess communication
	// TODO: Tasking
	// TODO: Load & run /sbin/init

	while (1);

	panic("kmain() reached end.");
}

