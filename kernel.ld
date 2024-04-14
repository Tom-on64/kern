SECTIONS {
    .text 0x1000 : {
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
