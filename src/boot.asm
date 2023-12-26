[org 0x7c00]
[bits 16]
[global start]

start:
    ; Setup
    xor ax, ax
    mov es, ax
    mov ds, ax
    mov bp, 0x7c00
    mov sp, bp

    ; Save drive number
    mov byte [driveNumber], dl

    ; Print boot message
    mov si, bootMsg
    call print
    mov dl, byte [driveNumber]
    call printHex
    mov si, newline
    call print

    ; Read filetable from disk
    mov bx, FILETAB_LOC ; Filetable location in memory
    mov es, bx
    xor bx, bx

    mov ah, 2           ; Read from disk
    mov al, 1           ; Read 1 sector (512B)

    mov dl, byte [driveNumber] ; Drive number
    mov ch, 0           ; Cylinder
    mov dh, 0           ; Head
    mov cl, 2           ; Sector
    int 0x13            ; INT 0x13 - Disk operations

    jc readFail         ; CF will be set if the read fails

    ; Read filetable from disk
    mov bx, KERNEL_LOC ; Filetable location in memory
    mov es, bx
    xor bx, bx

    mov ah, 2           ; Read from disk
    mov al, 3           ; Read 3 sectors (1.5kB)

    mov dl, byte [driveNumber] ; Drive number
    mov ch, 0           ; Cylinder
    mov dh, 0           ; Head
    mov cl, 3           ; Sector
    int 0x13            ; INT 0x13 - Disk operations

    jc readFail         ; CF will be set if the read fails

    ; Setup segmentation
    mov ax, KERNEL_LOC
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Setup stack
    mov bp, 0xffff
    mov sp, bp
    mov ax, 0x9000
    mov ss, ax

    mov bl, byte [driveNumber]

    ; Setup video mode
    mov ax, 0x0003
    int 0x10
    mov ah, 0x0b
    mov bx, 0x0001
    int 0x10

    ; Far jump to the kernel (never returns)
    jmp KERNEL_LOC:0x0000

readFail:
    mov si, diskErrorMsg
    call print
    cli
    hlt

%include "./src/lib/print.asm"

driveNumber: db 0

bootMsg: db ENDL, "Booting kern.", ENDL, "Booting from drive ", 0
diskErrorMsg: db "Disk Error!", ENDL, 0

FILETAB_LOC equ 0x1000
KERNEL_LOC equ 0x2000

times 510-($-$$) db 0
dw 0xaa55
