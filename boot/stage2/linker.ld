OUTPUT_FORMAT(binary)
ENTRY(entry)

SECTIONS {
	/* Will be loaded at 0x70000 */
	. = 0x70000;

	.text : {
		*(.text)
		*(.text.*)
		*(.rodata)
		*(.rodata.*)
	}

	.data : {
		*(.data)
		*(.data.*)
	}

	.bss : {
		*(COMMON)
		*(.bss)
		*(.bss.*)
	}

	_end = .;
}
