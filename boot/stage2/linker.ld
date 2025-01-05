OUTPUT_FORMAT(binary)
OUTPUT_ARCH(i386)

SECTIONS {
	. = 0x70000;

	.text : {
		*(entry)
		*(.text)
	}

	.rodata : {
		*(.rodata)
	}

	.data : {
		*(.data)
	}

	.bss : {
		*(COMMON)
		*(.bss)
	}
}
