[org 0x7c00]
[bits 16]
[global start]

start:
    ; Setup
    xor ax, ax
    mov es, ax
    mov ds, ax
    mov bp, 0x9000
    mov sp, bp

    ; Read filetable from disk
    mov bx, FILETAB_LOC ; Filetable location in memory
    mov es, bx
    xor bx, bx

    mov ah, 2           ; Read from disk
    mov al, 1           ; Read 1 sector
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
    mov al, 2           ; Read 2 sectors
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
    mov ss, ax

    ; Far jump to the kernel
    jmp KERNEL_LOC:0x0

readFail:
    cli
    hlt

FILETAB_LOC equ 0x1000
KERNEL_LOC equ 0x2000

times 510-($-$$) db 0
dw 0xaa55
