#include <bootloader.h>
#include <kmalloc.h>
#include <syscall.h>
#include <kernel.h>
#include <paging.h>
#include <serial.h>
#include <timer.h>
#include <task.h>
#include <gdt.h>
#include <isr.h>
#include <pmm.h>
#include <tty.h>

__noreturn
void kmain(void* ptr, uint32_t magic) {
	// Serial console for debugging
	if (serial_init(COM1) != 0) panic("Failed to initialize Serial driver.");

	// Load structures from bootloader
	if (boot_init_mb1(ptr, magic) != 0) panic("Unsupported bootloader.");

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
	if (syscall_init() != 0) panic("Failed to initialize syscalls.");
	if (task_init() != 0) panic("Failed to initialize tasking.");

	// TODO: Load & run /sbin/init

	// We don't want to halt
	while (1);
}

