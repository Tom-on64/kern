#include <bootloader.h>
#include <kernel.h>
#include <paging.h>
#include <serial.h>
#include <splash.h>
#include <system.h>
#include <gdt.h>
#include <isr.h>
#include <pmm.h>

void a(void) { *(char*)(1234) = 0; } 

__noreturn
void _start(void) {
	int err = 0;

	// Serial console for debugging
	if ((err = serial_init(COM1)) < 0) panic("Failed to initialize Serial driver. (%d)", err);

	// Load structures from bootloader
	if ((err = boot_init()) < 0) panic("Failed to parse bootloader structures. (%d)", err);

	// CPUID thingy :p
	uint32_t eax = 0;
	char s[12];
	cpuid(&eax, (void*)&s[0], (void*)&s[8], (void*)&s[4]);
	pr_info("[kernel] CPUID_GETVENDORID: %s\n", s);

	// System init
	if ((err = gdt_init()) < 0) panic("Failed to initalize GDT. (%d)", err);
	if ((err = isr_init()) < 0) panic("Failed to initalize ISRs. (%d)", err);

	// Memory management
	if ((err = pmm_init()) < 0) panic("Failed to initalize Physical Memory Manager. (%d)", err);
	if ((err = pag_init()) < 0) panic("Failed to initalize Paging. (%d)", err);

	// The most important function in the world.
	splash();

	a();

	// Patiently wait for things to do :)
	while (1);
};

