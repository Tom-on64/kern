#include <bootloader.h>
#include <kernel.h>
#include <serial.h>
#include <tty.h>

/* TODO: Commented out stuff */

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

	// gdt_init();
	// isr_init();
	// pag_init();

	debugf("Basic initialization complete!\n");

	struct boot_memmap* entry;
	for (
		entry = (struct boot_memmap*)bootloader.memmap;
		(uint64_t)entry < bootloader.memmap + bootloader.memmapLen;
		entry = (struct boot_memmap*)((uint64_t)entry + entry->skip + sizeof(entry))
	) {

		debugf("[Region] base: %x size: %x b type: ", entry->base, entry->size);
		switch (entry->type) {
		case BOOT_MMAP_AVAILABLE:	debugf("Available\n"); break;
		case BOOT_MMAP_RESERVED:	debugf("Reserved\n"); break;
		case BOOT_MMAP_ACPI_RECLAIM:	debugf("ACPI Reclaim\n"); break;
		case BOOT_MMAP_NVS:		debugf("NVS\n"); break;
		case BOOT_MMAP_BADRAM:		debugf("Bad RAM\n"); break;
		default:			debugf("Invalid type\n"); break;
		}
	}
	
	panic("kmain() reached end.");

	// Temporary loop since panic() does not halt
	while (1);
}

