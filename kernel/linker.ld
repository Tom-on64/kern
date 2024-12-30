OUTPUT_FORMAT(elf32-i386)
ENTRY(start)

SECTIONS {
	. = 0x00100000;
	kernel_start = .;

	/* Multiboot Header */
	.multiboot ALIGN(4K) : {
		*(.multiboot)
	}

	.boot ALIGN(4K) : {
		*(.boot)
	}

	. += 0xC000000;

	/* Kernel Code */
	.text ALIGN(4) : AT(ADDR(.text) - 0xC000000) {
		*(.text)
		*(.text.*)
	}

	/* Read-Only Data */
	.rodata ALIGN(4) : AT(ADDR(.rodata) - 0xC000000) {
		*(.rodata)
		*(.rodata.*)
	}

	/* Read/Write Data */
	.data ALIGN(4) : AT(ADDR(.data) - 0xC000000) {
		*(.data)
		*(.data.*)
	}

	/* BSS Section */
	.bss ALIGN(4) : AT(ADDR(.bss) - 0xC000000) {
		*(COMMON)
		*(.bss)
		*(.bss.*)
	}

	/* Discarded Sections */
	/DISCARD/ : {
		*(.note)
		*(.note.*)
		*(.comment)
	}

	kernel_end = .;
}
