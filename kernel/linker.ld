OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386");
ENTRY(_start)

/* Define sections */
SECTIONS {
	. = 0x00100000;
	kernel_start = .;

	.boot ALIGN(4K) : {
		*(.multiboot)
		*(.boot)
	}

	. += 0xC0000000;

	.text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000) {
		*(.text)
	}

	.rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000) {
		*(.rodata)
	}

	.data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000) {
		*(.data)
	}

	.bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000) {
		*(COMMON)
		*(.bss)
	}

	kernel_end = .;
}
