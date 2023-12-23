[bits 16]

start:
    mov si, msg
    call print

    xor ax, ax
    int 0x16

.return:
    mov ax, 0x2000
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x2000:0x0000

%include "./src/print.asm"

msg: db "Hello!", ENDL, 0

times 512-($-$$) db 0
