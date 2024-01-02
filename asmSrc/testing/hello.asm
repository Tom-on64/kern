[bits 16]

start:
    mov ah, 0x0e
    mov al, 'H'
    int 0x10
    mov al, 'e'
    int 0x10
    mov al, 'l'
    int 0x10
    int 0x10
    mov al, 'o'
    int 0x10
    mov al, '!'
    int 0x10

    xor ax, ax
    int 0x16

    mov ax, 0x2000
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    jmp 0x2000:0x0000
