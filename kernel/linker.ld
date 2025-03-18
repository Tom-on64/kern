OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386");
ENTRY(_start)

/* Define correct program headers */
PHDRS {
	text	PT_LOAD		FLAGS((1 << 0) | (1 << 2));	/* Read, Execute */
	rodata	PT_LOAD		FLAGS((1 << 0));		/* Read */
	data	PT_LOAD		FLAGS((1 << 0) | (1 << 1));	/* Read, Write */
}

/* Define sections */
SECTIONS {
	/* 1MB is a conventional place for kernels */
	. = 1M;

	__kernel_start = .;
	.text BLOCK(4K) : ALIGN(4K) {
		*(.multiboot)
		*(.text)
	} : text

	.rodata BLOCK(4K) : ALIGN(4K) {
		*(.rodata)
	} : rodata

	.data BLOCK(4K) : ALIGN(4K) {
		*(.data)
	} : data

	.bss BLOCK(4K) : ALIGN(4K) {
		*(COMMON)
		*(.bss)
	} : data
	__kernel_end = .;

	/DISCARD/ : {
		*(.eh_frame)
		*(.note*)
	}
}
