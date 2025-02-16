OUTPUT_FORMAT("elf32-i386")
ENTRY(_start)

SECTIONS {
	/* 1MB is a conventional place for kernels */
	. = 1M;

	/* We need the multiboot header in the first 8192 bytes */
	.text BLOCK(4K) : ALIGN(4K) {
		*(.multiboot)
		*(.text)
	}

	/* Read-only data */
	.rodata BLOCK(4K) : ALIGN(4K) {
		*(.rodata)
	}

	/* Initalized r/w data */
	.data BLOCK(4K) : ALIGN(4K) {
		*(.data)
	}

	/* Uninitialized r/w data and stack */
	.bss BLOCK(4K) : ALIGN(4K) {
		*(COMMON)
		*(.bss)
	}

	. = 4M;
}
