#include <bootloader.h>
#include <kernel.h>
#include <limine.h>
#include <paging.h>
#include <serial.h>
#include <splash.h>
#include <timer.h>
#include <gdt.h>
#include <idt.h>
#include <isr.h>
#include <pmm.h>

static volatile LIMINE_BASE_REVISION(4);

__noreturn
void _start(void) {
	// Serial console for debugging
	if (serial_init(COM1) != 0) panic("Failed to initialize Serial driver.");

	if (LIMINE_BASE_REVISION_SUPPORTED == false) panic("Unsuported Limine version.");

	// Load structures from bootloader
	if (boot_init() != 0) panic("Failed to parse bootloader structures.");

	// The most important function in the world.
	splash();

	// System init
	if (gdt_init() != 0) panic("Failed to initalize GDT.");
	if (isr_init() != 0) panic("Failed to initalize ISRs.");
	if (pag_init() != 0) panic("Failed to initalize Paging.");

	// Memory manager init
	//if (pmm_init() != 0) panic("Failed to initalize Physical Memory Manager.");
	//if (kmalloc_init() != 0) panic("Failed to initialize kernel heap.");

	//if (timer_init() != 0) panic("Failed to initialize timer.");
	//if (syscall_init() != 0) panic("Failed to initialize syscalls.");
	//if (task_init() != 0) panic("Failed to initialize tasking.");

	// TODO: Load & run /sbin/init

	// We don't want to halt
	while (1);
}

