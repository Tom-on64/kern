SECTIONS {
    .text 0xC0000000 : {
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
