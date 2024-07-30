[bits 16]
[org 0x7e00]

%define STAGE3_LOC 0x50000
%define MODE_INFO_BLOCK_LOC 0x6000

start:
    mov [driveNum], dl

    ; Physical memory map setup
    xor ax, ax
    mov es, ax

memMapEntries equ 0x7000
getMemoryMap:
    mov di, memMapEntries + 4 ; 4 bytes after memMapEntries
    xor ebx, ebx
    xor bp, bp
    mov edx, "PAMS" ; SMAP in little endian
    mov eax, 0xe820
    mov dword [es:di+20], 1 ; Force valid ACPI 3.X entry
    mov ecx, 24
    int 0x15
    jc .error

    cmp eax, "PAMS"
    jne .error
    test ebx, ebx
    jnz .start

.error:
    stc
    jmp inputGfx

.nextEntry:
    mov edx, "PAMS" ; In case BIOS messes with edx
    mov ecx, 24
    mov eax, 0xe820
    int 0x15

.start:
    jcxz .skipEntry
    mov ecx, [es:di+8] ; Low 32 bits of length
    or ecx, [es:di+12] ; High 32 bits of length
    jz .skipEntry

    ; Valid entry
    inc bp
    add di, 24 ; Next entry

.skipEntry:
    test ebx, ebx
    jz .done
    jmp .nextEntry

.done:
    mov [memMapEntries], bp
    clc

;; Get graphics values from user (resolution etc.)
inputGfx:
    xor ecx, ecx
    cmp word [width], 0 ; Values already set
    jne vbeSetup
    mov si, gfxChooseMsg
    call print
    call getNumber ; Stores input value in bx
    mov [width], bx

    mov si, gfxHeightMsg
    call print
    call getNumber
    mov [height], bx

    mov si, gfxBppMsg
    call print
    call getNumber
    mov [bpp], bx

vbeSetup:
    ; Setup VBE info
    xor ax, ax
    mov es, ax
    mov ax, 0x4f00
    mov di, vbeInfoBlock
    int 0x10

    cmp ax, 0x4f
    jne error

    mov ax, word [vbeInfoBlock.videoModePtr]
    mov [offset], ax
    mov ax, word [vbeInfoBlock.videoModePtr+2]
    mov [_segment], ax

    mov fs, ax
    mov si, [offset]

;; Get next VBE mode
.findMode:
    mov dx, [fs:si]
    inc si
    inc si
    mov [offset], si
    mov [mode], dx

    cmp dx, 0xffff ; Indicates end of modes
    je .endOfModes

    mov ax, 0x4f01 ; Get VBE mode info
    mov cx, [mode]
    mov di, modeInfoBlock
    int 0x10

    cmp ax, 0x4f
    jne error

    ; Print values (Debug)
    ; mov ax, [modeInfoBlock.xRes]
    ; ror ax, 8
    ; call printHex
    ; ror ax, 8
    ; call printHex
    ; mov ax, 0x0e20
    ; int 0x10
    ; mov ax, [modeInfoBlock.yRes]
    ; ror ax, 8
    ; call printHex
    ; ror ax, 8
    ; call printHex
    ; mov ax, 0x0e20
    ; int 0x10
    ; xor ax, ax
    ; mov al, [modeInfoBlock.bpp]
    ; call printHex
    ; mov ax, 0x0e20
    ; int 0x10 
    ; mov al, 0x0a
    ; int 0x10
    ; mov al, 0x0d
    ; int 0x10

    ; Check if it is the right mode
    mov ax, [width]
    cmp ax, [modeInfoBlock.xRes]
    jne .nextMode

    mov ax, [height]
    cmp ax, [modeInfoBlock.yRes]
    jne .nextMode
    
    mov al, [bpp]
    cmp al, [modeInfoBlock.bpp]
    jne .nextMode

    ; Set VBE mode
    mov ax, 0x4f02
    mov bx, [mode]
    or bx, 0x4000 ; Set 14th bit to 1
    xor di, di
    int 0x10

    cmp ax, 0x4f
    jne error

    jmp loadGDT

.nextMode:
    mov ax, [_segment]
    mov fs, ax
    mov si, [offset]
    jmp .findMode
    
.endOfModes:
    mov ax, 0x03
    int 0x10 ; Clear screen
    mov si, gfxModeNotFoundErr
    call print
    mov word [width], 0 ; Unset values
    jmp inputGfx

error:
    mov si, vbeErr
    call print
    cli
    hlt

;; Load the global descriptor table (GDT)
loadGDT:
    cli
    lgdt [GDT_Descriptor]
    ; Enable protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Far jump to 32 bit mode!!
    jmp CODE_SEG:protected_start

;;
;; Prints a hex byte
;; al - value to print
;;
printHex:
    push ax
    push bx
    
    mov bh, 2
    mov bl, al
    and al, 0xf0
    ror al, 4
    mov ah, 0x0e ; For printing
.printNibble:
    cmp al, 10
    jl .print
    add al, 39
.print:
    add al, 48 ; ASCII Char
    int 0x10
    dec bh
    jz .done
    mov al, bl
    and al, 0x0f
    jmp .printNibble
.done:
    pop bx
    pop ax
    ret

;;
;; Prints a null terminated string
;; si - pointer to string
;;
print:
    push ax
    mov ah, 0x0e
.loop:
    lodsb ; mov al, [si]   inc si
    cmp al, 0
    je .done
    int 0x10
    jmp .loop
.done:
    pop ax
    ret

;;
;; Reads a base 10 number as input from user
;; Returns:
;;  bx - number read
;;
getNumber: ; TODO: if user enters a number that overflows (n > 0xffff) shit breaks
    push cx
    push ax
    push dx
    xor bx, bx
    mov cx, 10
.nextDigit:
    xor ah, ah
    int 0x16
    mov ah, 0x0e

    cmp al, 0x08 ; Backspace
    je .backspace
    cmp al, 0x0d ; Enter
    je .enter

    cmp al, '0' ; Make sure user entered a number
    jl .nextDigit
    cmp al, '9' 
    jg .nextDigit

    int 0x10 ; Print the digit

    sub al, '0' ; Convert to integer
    mov ah, 0
    imul bx, bx, 10 ; bx *= 10
    add bx, ax ; bx += ax
    jmp .nextDigit

.backspace:
    cmp bx, 0 ; Check if we can backspace
    je .nextDigit 
    int 0x10 ; go back one space

    mov ax, bx
    xor dx, dx
    div cx      ; dx:ax / cx -> ax = result, dx = remainder
    mov bx, ax

    mov ah, 0x0e
    mov al, ' ' 
    int 0x10
    mov al, 0x08
    int 0x10

    jmp .nextDigit

.enter:
    pop dx
    pop ax
    pop cx
    ret

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

    ; Store VBE Mode Info Block in memory
    mov esi, modeInfoBlock
    mov edi, MODE_INFO_BLOCK_LOC ; Destination
    mov ecx, 64 ; 64 * 4 = 256B (size of modeInfoBlock)
    rep movsd

    ; Perform The far jump
    jmp CODE_SEG:STAGE3_LOC

;; Data
driveNum: db 0

vbeErr: db "VBE Error!", 0
acpiFuncErr: db "ACPI Function not supported!", 0

gfxChooseMsg: db "Please enter desired values" ; No NULL terminator, we want to print gfxWidthMsg right after anyway
gfxWidthMsg: db 0x0a, 0x0d, "Width: ", 0
gfxHeightMsg: db 0x0a, 0x0d, "Height: ", 0
gfxBppMsg: db 0x0a, 0x0d, "Bits Per Pixel: ", 0
gfxModeNotFoundErr: db "Mode not found! ", 0

;; GDT
GDT:
.start:
    .nullDescriptor:    ; No clue why this exists
        dd 0
        dd 0
    .codeDescriptor:    ; Memory for code
        dw 0xffff       ; First 16 bits of limit
        dw 0            ; 16 bits +
        db 0            ; + 8 bits = 24 bits
        db 0b10011010   ; p, dpl, s, type flags
        db 0b11001111   ; Other flags, last 4 bits of limit
        db 0            ; Base
    .dataDescriptor:    ; Memory for data
        dw 0xffff       ; First 16 bits of limit
        dw 0            ; 16 bits +
        db 0            ; + 8 bits = 24 bits
        db 0b10010010   ; p, dpl, s, type flags
        db 0b11001111   ; Other flags, last 4 bits of limit
        db 0            ; Base
.end:

GDT_Descriptor:
    dw GDT.end - GDT.start - 1  ; Size
    dd GDT.start        ; GDT Pointer

CODE_SEG equ GDT.codeDescriptor - GDT.start
DATA_SEG equ GDT.dataDescriptor - GDT.start

;; VBE Stuff
;; !! IF YOU WANT TO SELECT ANY VALUE IN THE SETUP, SET THESE TO ZERO !!
width: dw 1920
height: dw 1080
bpp: db 32

offset: dw 0
_segment: dw 0 ; segment is a keyword
mode: dw 0

times (512*2)-($-$$) db 0 ; End Sector 2

;; Sector 2
vbeInfoBlock:
    .vbeSigniture: db "VBE2"
    .vbeVersion: dw 0x0300 ; BCD Version
    .oemStringPtr: dd 0
    .capibilities: dd 0
    .videoModePtr: dd 0
    .totalMemory: dw 0
    .oemSoftwareRev: dw 0
    .oemVendorNamePtr: dd 0
    .oemProductNamePtr: dd 0
    .oemProductRevisionPtr: dd 0
    .reserved: times 222 db 0
    .oemData: times 256 db 0

;; Sector 3
modeInfoBlock:
;; Required by all VBE revisions
    .modeAttr: dw 0
    .windowAAtrr: db 0
    .windowBAttr: db 0
    .windowGranularity: dw 0
    .windowSize: dw 0
    .windowASegment: dw 0
    .windowBSegment: dw 0
    .windowFunctionPtr: dd 0
    .bytesPerScanline: dw 0

;; Required by VBE 1.2+
    .xRes: dw 0
    .yRes: dw 0
    .xCharSize: db 0
    .yCharSize: db 0
    .planeCount: db 0
    .bpp: db 0
    .bankCount: db 0
    .memoryModel: db 0
    .bankSize: db 0
    .imagePageCount: db 0
    .reserved1: db 1

;; Direct color fields
    .redMaskSize: db 0
    .redFieldPos: db 0
    .greenMaskSize: db 0
    .greenFieldPos: db 0
    .blueMaskSize: db 0
    .blueFieldPos: db 0
    .reservedMaskSize: db 0
    .reservedFieldPos: db 0
    .directColorModeInfo: db 0

;; Required by VBE 2.0+
    .physicalBasePtr: dd 0
    .reserved2: dd 0
    .reserved3: dw 0

;; Required by VBE 3.0+
    .linearBytesPerScanline: dw 0
    .bankImagePageCount: db 0
    .linearImagePageCount: db 0
    .linearRedMaskSize: db 0
    .linearRedFieldPos: db 0
    .linearGreenMaskSize: db 0
    .linearGreenFieldPos: db 0
    .linearBlueMaskSize: db 0
    .linearBlueFieldPos: db 0
    .linearReservedMaskSize: db 0
    .linearReservedFieldPos: db 0
    .maxPixelClock: dd 0

    .reserved4: times 190 db 0 

times (512*4)-($-$$) db 0

