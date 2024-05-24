ENTRY("main");
SECTIONS {
    .text : {
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

