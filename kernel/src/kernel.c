#include <multiboot.h>
#include <kernel.h>
#include <serial.h>
#include <pmm.h>
#include <tty.h>

#define _hlt()	while (1) __asm__ volatile ("cli; hlt");

char* itoa(uint8_t n) {
	static char str[32];
	int i = sizeof(str) - 2;
	while (n) {
		str[i--] = (n % 10) + '0';
		n /= 10;
	}
	str[sizeof(str) - 1] = '\0';
	return &str[i+1];
}

__noreturn
void kmain(void* ptr) {
	multiboot_info_t* mbi = ptr;

	// Serial console for debugging
	serial_init(COM1);
	serial_puts(COM1, "\x1b[0H\x1b[Jkern.\n");

	// Check if we have a valid memmap
	if (!(mbi->flags >> 6 & 1)) {
		panic("Invalid memory map.");
	}

	// VGA tty thingy
	tty_init();
	tty_puts("kern.");

	// Memory manager init
	// pmm_init();
	// vmm_init();

	// x86 shit
	// gdt_init();
	// isr_init();
	
	/* TODO: Commeted out stuff */

	panic("kmain() reached end.");
}

