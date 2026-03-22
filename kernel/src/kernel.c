#include <bootloader.h>
#include <kernel.h>
#include <limine.h>
#include <paging.h>
#include <serial.h>
#include <splash.h>
#include <system.h>
#include <timer.h>
#include <gdt.h>
#include <idt.h>
#include <isr.h>
#include <pmm.h>

__noreturn
void _start(void) {
	// Serial console for debugging
	if (serial_init(COM1) != 0) panic("Failed to initialize Serial driver.");

	// Load structures from bootloader
	if (boot_init() != 0) panic("Failed to parse bootloader structures.");

	// The most important function in the world.
	splash();

	// CPUID thingy :p
	uint32_t eax = 0;
	char s[12];
	cpuid(&eax, (void*)&s[0], (void*)&s[8], (void*)&s[4]);
	debugf("[kernel] CPUID_GETVENDORID: %s\n", s);

	// System init
	if (gdt_init() != 0) panic("Failed to initalize GDT.");
	if (isr_init() != 0) panic("Failed to initalize ISRs.");

	// Memory management
	if (pmm_init() != 0) panic("Failed to initalize Physical Memory Manager.");
	if (pag_init() != 0) panic("Failed to initalize Paging.");
	//if (kmalloc_init() != 0) panic("Failed to initialize kernel heap.");

	// Tasking
	//if (timer_init() != 0) panic("Failed to initialize timer.");
	//if (syscall_init() != 0) panic("Failed to initialize syscalls.");
	//if (task_init() != 0) panic("Failed to initialize tasking.");

	// TODO: Load & run /sbin/init

	// We don't want to halt
	while (1);
}

