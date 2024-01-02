[bits 16]

start:
    pusha
    
    mov si, msg
    call print

    xor ax, ax
    int 0x16

    mov dl, ah
    call printHexByte

    xor ax, ax
    int 0x16

    popa
.return:
    mov ax, 0x2000
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    jmp 0x2000:0x0000

%include "./src/lib/print.asm"

msg: db "Hello!", ENDL, 0

times 512-($-$$) db 0
