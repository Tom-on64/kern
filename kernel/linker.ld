OUTPUT_FORMAT(elf32-i386)
ENTRY(kmain)

SECTIONS {
	. = 0x00100000;

	/* Multiboot Header */
	.multiboot ALIGN(4) : {
		*(.multiboot)
	}

	/* Kernel Code */
	.text ALIGN(4) : {
		*(.text)
		*(.text.*)
	}

	/* Read-Only Data */
	.rodata ALIGN(4) : {
		*(.rodata)
		*(.rodata.*)
	}

	/* Read/Write Data */
	.data ALIGN(4) : {
		*(.data)
		*(.data.*)
	}

	/* BSS Section */
	.bss ALIGN(4) : {
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
}
