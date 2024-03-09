[bits 32]
[global _start]

[extern main]
[extern idtp]

_start:
    call main
    cli
    hlt

[global idtLoad]
idtLoad:
    lidt [idtp]
    ret
