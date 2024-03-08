;
; Loads a file from the filetable into a memory location
; Params:
;   dl - Disk number (byte)
;   Filename (Null terminated)
;   Destination location segment
;   Destination location offset
;
; Returns:
;   al - Exit code : 0 - Success; 1 - File not found; 2 - Disk error
;
loadFile:
    ; Func prologue
    push bp
    mov bp, sp
    push bx
    push fs
    push cx
    push dx
    push si
    push es

    mov byte [diskNum], dl
    mov bx, [bp+4]      ; src Offset
    mov fs, [bp+6]      ; src Segment
    mov si, [bp+8]      ; Filename

    push bx

    mov cx, 10
    mov di, fileName
.getFilename:
    lodsb           ; mov byte al, [si]; inc si
    cmp al, 0       ; End of fileName
    je .filenamePad
    stosb
    loop .getFilename
    jmp .filenameDone
.filenamePad: 
    dec si          ; Fill the rest with Null
.filenamePadLoop:
    stosb
    loop .filenamePadLoop
.filenameDone:
    mov si, fileName
    call print

    mov ax, FILETAB_LOC
    mov es, ax
    xor di, di      ; es:di - Filetable location
    mov si, fileName
.fileNameCheck:
    mov al, [es:di]
    cmp al, 0       ; End of filetable
    je .notFound

    cmp al, [si]
    je .beginCmp

    add di, 16
    jmp .fileNameCheck
.beginCmp:
    push di
    mov cl, 10
.cmpLoop:
    mov al, [es:di]
    inc di
    cmp al, [si]
    jne .redoCheck

    dec cl 
    jz .found
    inc si
    jmp .cmpLoop
.redoCheck:
    mov si, fileName
    pop di
    add di, 16
    jmp .fileNameCheck
.notFound:
    mov byte [exitCode], NOT_FOUND
    jmp .done
.found:
    add di, 3       ; Skip extension
    inc di          ; Skip Number of entries

    xor ax, ax      ; Reset disk
    mov dl, [diskNum]
    int 0x13

    mov ch, 0       ; Cylinder
    mov dh, 0       ; Head
    mov cl, [es:di] ; Sector
    inc di
    mov al, [es:di] ; Length in sectors
    mov bx, fs
    mov es, bx
    pop bx
    mov dx, bx
    call printHexByte
    mov ah, 2       ; Read from disk
    int 0x13

    jnc .done

    mov byte [exitCode], DISK_ERROR
.done:
    ; Func epilogue
    pop es
    pop si
    pop dx
    pop cx
    pop fs
    pop bx
    mov sp, bp
    pop bp
    mov byte al, [exitCode]
    ret

;
; Saves a file to the disk and updates filetable
; dl - Disk number
; Params:
;   Filename (Null terminated)
;   Filetype (Null terminated)
;   Filesize (Byte)
;   Source location segment
;   Source location offset
;
; Returns:
;   al - Exit code : 0 - Success; 1 - File not found; 2 - Disk error
;
saveFile:
    ; Func prologue
    push bp
    mov bp, sp
    push bx
    push fs
    push cx
    push dx
    push si
    push es

    mov byte [diskNum], dl
    mov bx, [bp+4]      ; src Offset
    mov fs, [bp+6]      ; src Segment
    mov cx, [bp+8]      ; Filesize 
    mov ax, [bp+10]     ; Filetype

    push bx

    mov byte [filesize], cl

    mov si, ax
    mov di, filetype
    mov cx, 3
    rep movsb

    mov si, [bp+12] ; Filename
    mov cx, 10
    mov di, fileName
.getFilename:
    lodsb           ; mov byte al, [si]; inc si
    cmp al, 0       ; End of fileName
    je .filenamePad
    stosb
    loop .getFilename
    jmp .filenameDone
.filenamePad: 
    dec si          ; Fill the rest with Null
.filenamePadLoop:
    stosb
    loop .filenamePadLoop
.filenameDone:
    mov ax, FILETAB_LOC
    mov es, ax
    xor di, di      ; es:di - Filetable location
    mov si, fileName
.fileNameCheck:
    mov byte al, [es:di+14] ; Starting sector
    cmp al, 0       ; No file, create one
    je createFile

    add byte al, [es:di+15] ; Filesize 
    mov byte [availableSector], al

    mov al, [es:di]
    cmp al, 0       ; End of filetable
    je createFile   ; File doesn't exist, create new one

    cmp al, [si]
    je .beginCmp

    add di, 16
    jmp .fileNameCheck
.beginCmp:
    push di
    mov cl, 10
.cmpLoop:
    mov al, [es:di]
    inc di
    cmp al, [si]
    jne .redoCheck

    dec cl 
    jz .found
    inc si
    jmp .cmpLoop
.redoCheck:
    mov si, fileName
    pop di
    add di, 16
    jmp .fileNameCheck
.found:             ; File already exists (override)
    ; TODO: Ask user if they want to override file
    add di, 3       ; Skip extension
    inc di          ; Skip Number of entries

    xor ax, ax      ; Reset disk
    mov dl, [diskNum]
    int 0x13

    ; TODO: Override current file
    ; push fs
    ; mov ch, 0       ; Cylinder
    ; mov dh, 0       ; Head
    ; mov cl, [es:di] ; Sector
    ; inc di
    ; mov al, [es:di] ; Length in sectors
    ; mov ah, 3       ; Write to disk
    ; pop es          ; es:bx - Destination
    ; int 0x13

    ; jnc returnToCaller

    mov byte [exitCode], DISK_ERROR
    jmp returnToCaller  ; TODO: Fix this

createFile:             ; Appends a file to the filetable.
    mov di, FILETAB_LOC
    mov es, di
    xor di, di

.filetabEnd:            ; Move to the end of filetable
    cmp byte [es:di], 0
    je .filetabEndDone
    add di, 16
    jmp .filetabEnd
.filetabEndDone:
    mov cx, 10
    mov si, fileName
    rep movsb           ; Append the fileName
    mov cx, 3
    mov si, filetype
    rep movsb           ; Append the extension
    mov byte [es:di], 0 ; Number of entries
    inc di
    mov byte al, [availableSector]
    stosb               ; Starting sector
    mov byte al, [filesize]
    stosb               ; File length in sectors

    ; Write changed filetable to disk
    ;! If filetable size changes, this will have to change (probably)
    mov bx, FILETAB_LOC
    mov es, bx
    xor bx, bx          ; es:bx - Filetable location
    mov dl, [diskNum]
    mov ah, 3           ; Write to disk
    mov al, 1           ; Write 1 sector
    mov ch, 0           ; Cylinder
    mov dh, 0           ; Head
    mov cl, 2           ; Sector
    int 0x13

    jnc writeFileData

    mov byte [exitCode], DISK_ERROR
    jmp returnToCaller

writeFileData:          ; Writes file data to the disk
    mov ax, 0x8000          ; Source segment
    mov es, ax
    pop bx              ; es:bx - Source
    mov ah, 3           ; Write to disk
    mov byte al, [filesize]
    mov dl, [diskNum]
    mov ch, 0           ; Cylinder
    mov dh, 0           ; Head
    mov byte cl, [availableSector]
    int 0x13

    jnc returnToCaller

    mov byte [exitCode], DISK_ERROR  ; Exit code - Disk Error

returnToCaller:
    ; Func epilogue
    pop es
    pop si
    pop dx
    pop cx
    pop fs
    pop bx
    mov sp, bp
    pop bp
    mov byte al, [exitCode]
    ret

;
; Removes a file from the disk/filetable
;
; Params:
;   Filename
;
removeFile:
    ret
    
;; Variables
availableSector: db 0
exitCode: db 0

diskNum: db 0
filesize: db 0
filetype: times 3 db 0
fileName: times 10 db 0

;; Constants
FILETAB_LOC equ 0x1000

; Exit codes
SUCCESS equ 0
NOT_FOUND equ 1
DISK_ERROR equ 2
