[org 0x7c00]
[bits 16]
[global start]

%define ENDL 0x0A, 0x0D

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
    call printHexByte
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
    mov al, 4           ; Read 4 sectors (2kB)

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

;
; Prints a string
; si - Pointer to NULL terminated string
;
print:
    pusha
    mov ah, 0x0e
    mov bl, 0x1f
    mov bh, 0
.loop:
    lodsb           ; Load [si] into al; Increment si
    or al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    popa
    ret    

;
; Prints a hex byte
; dl - Hex byte
;
printHexByte:
    pusha
    xor cx, cx
    mov bh, 0
    mov bl, 0x1f    ; White on blue
.loop:
    mov al, dl
    and al, 0xF0
    ror al, 4
    add al, '0'
    cmp al, '9'
    jle .hexNum
    add al, 7
.hexNum:
    mov ah, 0x0e
    int 0x10
    inc cx
    cmp cx, 2
    je .done
    ror dl, 4
    jmp .loop
.done:
    popa
    ret

driveNumber: db 0

bootMsg: db ENDL, "Booting kern.", ENDL, "Booting from drive 0x", 0
diskErrorMsg: db "Disk Error!", ENDL, 0

newline: db ENDL

FILETAB_LOC equ 0x1000
KERNEL_LOC equ 0x2000

times 510-($-$$) db 0
dw 0xaa55
