[bits 16]
[org 0x7c00]
[global start]

%define KERNEL_LOC 0x1000
%define KERNEL_SIZE 20      ; 20 Sectors -> 10kB
%define ENDL 0x0a, 0x0d
%define VIDMEM 0xb800

start:
    mov [driveNum], dl   ; Store the drive number

    ; Set up the stack
    xor ax, ax
    mov es, ax
    mov ds, ax
    mov bp, 0x7c00
    mov sp, bp

    ; Setup screen
    mov ax, 3       ; 80x25 Text mode
    int 0x10

    mov si, msg
    call puts

    ; Reset disk
    mov ah, 0x0
    mov dl, [driveNum]
    int 0x13
.read:
    ; Location
    mov bx, KERNEL_LOC

    ; CHS Location
    mov al, KERNEL_SIZE
    mov ch, 0   ; Cylinder
    mov dh, 0   ; Head
    mov cl, 3   ; Sector - 1 is this file, 2 is filetable
    mov dl, [driveNum]

    mov ah, 2   ; Read from disk
    int 0x13    ; Read interrupt

    jc .diskErr

    cli
    lgdt [GDT_Descriptor]
    ; Enable protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Far jump to 32 bit mode!!
    jmp CODE_SEG:protected_start

.diskErr:
    dec byte [counter]
    jnz .read

    mov si, diskErrMsg
    mov bl, 0x04
    call puts

    cli
    hlt

;
; Prints a string
; si: char* - String to print
; bl: byte  - Color attribute
;
puts:
    push ax
    push es

    mov ax, VIDMEM
    mov es, ax
    mov word di, [cursor]

    mov ah, bl
.loop:
    lodsb               ; mov al, [si]; inc si
    cmp al, 0
    je .done
    mov [es:di], ax     ; Really smart thing
    add di, 2
    jmp .loop
.done:
    mov [cursor], di
    pop es
    pop ax
    ret

;; Data
driveNum: db 0
counter: db 0
cursor: dw 0

; Strings
msg: db "Booting...", 0
diskErrMsg: db "Failed while reading from disk!", 0

;; 32 Bit entry code
[bits 32]
protected_start:
    ; Set up data and stack segments
    mov ax, DATA_SEG 
    mov ds, ax 
    mov ss, ax 
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Set up the stack pointer in protected mode
    mov ebp, 0x90000
    mov esp, ebp

    ; Perform the far jump
    mov al, [driveNum]
    push ax
    jmp KERNEL_LOC
    
;; GDT
GDT_Start:
    null_descriptor:    ; No clue why this exists
        dd 0
        dd 0
    code_descriptor:    ; Memory for code
        dw 0xffff       ; First 16 bits of limit
        dw 0            ; 16 bits +
        db 0            ; + 8 bits = 24 bits
        db 0b10011010   ; p, dpl, s, type flags
        db 0b11001111   ; Other flags, last 4 bits of limit
        db 0            ; Base
    data_descriptor:    ; Memory for data
        dw 0xffff       ; First 16 bits of limit
        dw 0            ; 16 bits +
        db 0            ; + 8 bits = 24 bits
        db 0b10010010   ; p, dpl, s, type flags
        db 0b11001111   ; Other flags, last 4 bits of limit
        db 0            ; Base
GDT_End:

GDT_Descriptor:
    dw GDT_End - GDT_Start - 1  ; Size
    dd GDT_Start        ; GDT Pointer

CODE_SEG equ code_descriptor - GDT_Start
DATA_SEG equ data_descriptor - GDT_Start

times 510-($-$$) db 0
dw 0xaa55

