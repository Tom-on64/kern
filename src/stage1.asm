[bits 16]
[org 0x7c00]
[global start]

%define VIDMEM 0xb800
%define KERNEL_LOC 0x1000
%define KERNEL_SIZE 30 ; 15kB

start:
    mov [driveNum], dl   ; Store the drive number

    ; Clear screen
    mov ax, 0x03
    int 0x10

    ; Reset disk
    mov ah, 0x0
    mov dl, [driveNum]
    int 0x13

.read:
    ; Location
    mov ax, 0 
    mov es, ax
    mov bx, 0x7e00 ; 2nd stage bootloader location
    
    ; CHS Location
    mov al, 3   ; Number of sectors to read (2 = 1kB)
    mov ch, 0   ; Cylinder
    mov dh, 0   ; Head
    mov cl, 2   ; Sector
    mov dl, [driveNum]

    mov ah, 2   ; Read from disk
    int 0x13    ; Read interrupt

    jnc loadKernel

    dec byte [counter]
    jnz .read

diskErr:
    mov si, diskErrMsg
    mov bl, 0x04
    mov ax, VIDMEM
    mov es, ax
    xor di, di
.printLoop:
    movsb
    mov [es:di], bl
    inc di
    cmp byte [si], 0
    jne .printLoop

    cli
    hlt

loadKernel:
    mov byte [counter], 3

.read:
    ; Location
    xor ax, ax 
    mov es, ax
    mov bx, KERNEL_LOC ; 2nd stage bootloader location
    
    ; CHS Location
    mov al, KERNEL_SIZE
    mov ch, 0   ; Cylinder
    mov dh, 0   ; Head
    mov cl, 6   ; Sector
    mov dl, [driveNum]

    mov ah, 2   ; Read from disk
    int 0x13    ; Read interrupt
    
    jnc nextStage
    dec byte [counter]
    jnz .read

;; Far jump to second stage bootloader
nextStage:
    mov dl, [driveNum]
    jmp 0x0000:0x7e00

;; Data
driveNum: db 0
counter: db 3

; Strings
diskErrMsg: db "Failed while reading from disk!", 0

times 510-($-$$) db 0
dw 0xaa55

