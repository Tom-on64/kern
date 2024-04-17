[bits 16]
[org 0x7e00]

%define KERNEL_LOC 0x1000

start:
    mov [driveNum], dl

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

    ; Perform The far jump
    mov al, [driveNum]
    push ax
    
    jmp CODE_SEG:KERNEL_LOC

;; Data
driveNum: db 0

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
width: dw 1920
height: dw 1080
bpp: db 32
offset: dw 0
_segment: dw 0 ; segment is a keyword
mode: dw 0

times 512-($-$$) db 0 ; End Sector 1

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
    .yRex: dw 0
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
    .physicalAddressPtr: dd 0
    .reserved2: db 0
    .reserved3: db 0

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

times (512*3)-($-$$) db 0 ; 2 Pad out to 3 sectors

