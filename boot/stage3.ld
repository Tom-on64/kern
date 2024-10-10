/* Third stage "prekernel" */
SECTIONS {
    .text 0x50000 : {
        *(entry);
        *(.text*);
    }
    .data : {
        *(.data*);
    }
    .rodata : {
        *(.rodata*);
    }
    .bss : {
        *(.bss*);
    }
}
