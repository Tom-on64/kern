OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64);
ENTRY(_start)

/* Program headers for MMU permissions */
PHDRS {
	text	PT_LOAD	FLAGS((1 << 0) | (1 << 2));	/* xr */
	rodata	PT_LOAD FLAGS((1 << 2));		/* ro */
	data	PT_LOAD FLAGS((1 << 1) | (1 << 2));	/* rw */
}

SECTIONS {
	. = 0xffffffff80000000;

	__kernel_start = .;
	__kernel_text_start = .;
	.text : {
		*(.text .text.*)
	} :text
	__kernel_text_end = .;

	. = ALIGN(CONSTANT(MAXPAGESIZE));

	__kernel_rodata_start = .;
	.rodata : {
		*(.rodata .rodata.*)
	} :rodata
	__kernel_rodata_end = .;

	. = ALIGN(CONSTANT(MAXPAGESIZE));

	__kernel_data_start = .;
	.data : {
		*(.data .data.*)
	} :data

	/* NOTE: .bss should be the last thing mapped to :data */
	.bss : {
		*(.bss .bss.*)
		*(COMMON)
	} :data
	__kernel_data_end = .;

	/* Discard .note* and .eh_frame* (wtf is an eh_frame??) to prevent issues */
	/DISCARD/ : {
		*(.eh_frame)
		*(.note .note.*)
	}
	__kernel_end = .;
}
