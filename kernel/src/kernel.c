#include <bootloader.h>
#include <kmalloc.h>
#include <kernel.h>
#include <paging.h>
#include <serial.h>
#include <timer.h>
#include <task.h>
#include <gdt.h>
#include <isr.h>
#include <pmm.h>
#include <tty.h>

void test(void);

__noreturn
void kmain(void* ptr, uint32_t magic) {
	if (boot_init_mb1(ptr, magic) != 0) panic("Unsupported bootloader.");

	// Serial console for debugging
	if (serial_init(COM1) != 0) panic("Failed to initialize Serial driver.");
	debugf("\x1b[H\x1b[J\x1b[0mkern. \x1b[36m(serial console)\x1b[0m\n\n");

	// System init
	if (gdt_init() != 0) panic("Failed to initalize GDT.");
	debugf("[gdt] Segment selectors: NULL: 0x%04x, KCODE: 0x%04x, KDATA: 0x%04x, UCODE: 0x%04x, UDATA: 0x%04x, TSS: 0x%04x\n",
		GDT_NULL, GDT_KERNEL_CODE, GDT_KERNEL_DATA, GDT_USER_CODE, GDT_USER_DATA, GDT_TSS);
	if (isr_init() != 0) panic("Failed to initalize ISRs.");
	if (pag_init() != 0) panic("Failed to initalize Paging.");

	// Memory manager init
	if (pmm_init() != 0) panic("Failed to initalize Physical Memory Manager.");
	if (kmalloc_init() != 0) panic("Failed to initialize kernel heap.");

	// VGA TTY
	if (tty_init() != 0) panic("Failed to initalize TTY driver.");
	tty_puts("kern.\n\n");

	if (timer_init() != 0) panic("Failed to initialize timer.");
	if (task_init() != 0) panic("Failed to initialize tasking.");

	// Create a temporary test task
	task_create(task_create_id(), (uint32_t)test, 1, pag_get_pagedir(), 0, NULL);

	size_t delay = 0;
	while (1) {
		delay++;

		if (delay < 0x1FFFFFF) continue;
		delay = 0;

		tty_puts("[kernel] Hello!\n");
	}
	
	// TODO: Syscalls
	// TODO: Interprocess communication
	// TODO: Load & run /sbin/init

	panic("kmain() reached end.");
}

// An imaginary second task
void test(void) {
	size_t delay = 0;
	while (1) {
		delay++;

		if (delay < 0x2FFFFFF) continue;
		delay = 0;

		tty_puts("[test] Hello!\n");
	}
}

