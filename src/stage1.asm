[bits 16]
[org 0x7c00]

%define SUPERBLOCK 0x8c00
%define FIRST_INODE_BLOCK (SUPERBLOCK) + 12
%define STAGE3_INODE 0xb000 + 128
%define EXTENT0 (STAGE3_INODE) + 21      ; Offset of extent[0] in inode_t

start:
    xor ax, ax
    mov es, ax
    mov ds, ax     

    mov byte [driveNum], dl

    ; Clear screen
    mov ax, 0x03
    int 0x10

    ;; Read in the boot block and the superblock to memory for 2nd stage bootloader
    mov al, 15          ; 8 - 1 for boot block - bootsector & 8 for super block
    mov bl, al
    dec bl              ; Number of sectors - 1
    mov di, 0x7E00      ; Destination 0x7c00-0x8c00 = boot block, 0x8c00-0x9c00 = superblock

    mov dx, 0x1F6       ; flags
    mov al, 0xE0        ; 0b0111 0000, bits: 0-3 = LBA bits 24-27, 4 = drive, 5 = always set, 6 = set for LBA, 7 = always set
    out dx, al

    mov dx, 0x1F2       ; Sector count port
    mov al, 15          ; number of sectors to read
    out dx, al

    inc dx              ; 0x1F3 - LBA low
    mov al, 1           ; LBA 1 = 2nd disk sector (0-based), bits 0-7 of LBA
    out dx, al

    inc dx              ; 0x1F4 - LBA mid
    xor ax, ax          ; bits 8-15 of LBA
    out dx, al

    inc dx              ; 0x1F5 - LBA high
    ; xor ax, ax        ; bits 16-23 of LBA
    out dx, al

    inc dx
    inc dx              ; 0x1F7 - Command port
    mov al, 0x20        ; Read with retry
    out dx, al

    call loadSectorLoop

    ;; Parse superblock for first inode block, offset to reserved bootloader

    ;; Read first inode block to memory
    ;; Each disk block is 4KB or 8 sectors of 512 bytes
    imul cx, [FIRST_INODE_BLOCK], 8 ; CX = disk sector/LBA of first inode block

    mov bl, 7       ; 8 sectors for a block - 1
    mov di, 0xb000  ; Address to read into, first inode block = 0xB000-0xC000

    mov dx, 0x1F6   ; flags
    mov al, 0xE0
    out dx, al

    mov dx, 0x1F2     
    mov al, 8         
    out dx, al

    inc dx            
    mov al, cl        
    out dx, al

    inc dx            
    mov al, ch        
    out dx, al

    inc dx            
    xor ax, ax        
    out dx, al

    inc dx
    inc dx            
    mov al, 0x20       
    out dx, al

    call loadSectorLoop

    ;; Read boot loader inode to memory
    ;;  inode 2, each inode is 64 bytes in size
    imul ecx, [EXTENT0], 8                  ; first_block translated to disk sector/LBA
    imul ebx, [EXTENT0+4], 8                ; length_blocks translated to disk sectors/LBAs

    mov dx, 0x1F2
    mov al, bl   
    out dx, al

    inc dx       
    mov al, cl   
    out dx, al

    inc dx       
    mov al, ch   
    out dx, al

    inc dx       
    shr ecx, 16
    mov al, cl   
    out dx, al

    inc dx       
    and ch, 0x0F 
    mov al, ch   
    or al, 0xE0  
    out dx, al

    inc dx       
    mov al, 0x20
    out dx, al

    dec bl       
    mov ax, 0x5000
    mov es, ax   
    xor di, di   

    call loadSectorLoop

    xor ax, ax
    mov es, ax          ; ES = 0x0000

    ;; Jump to 2nd stage bootloader here, does not return
    mov dl, [driveNum]
    jmp 0x0000:0x7E00

    ;; Poll status port after reading 1 sector
loadSectorLoop:
    in al, dx           ; Status register (reading port 1F7h)
    test al, 8          ; Sector buffer requires servicing 
    je loadSectorLoop   ; Keep trying until sector buffer is ready

    mov cx, 256         ; # of words to read for 1 sector
    mov dx, 0x1F0       ; Data port, reading 
    rep insw            ; Read bytes from DX port # into DI, CX # of times
    
    ;; 400ns delay - Read alternate status register
    mov dx, 0x3F6
    in al, dx
    in al, dx
    in al, dx
    in al, dx

    cmp bl, 0           ; Sector # to still read
    je .return

    dec bl
    mov dx, 0x1F7
    jmp loadSectorLoop
.return:
    ret
            
;; VARIABLES
driveNum: db 0

;; Boot sector magic
times 510-($-$$) db 0   ; Padding
dw 0xaa55

