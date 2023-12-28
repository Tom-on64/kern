;
; Loads a file from the filetable into a memory location
; dl - Disk number
; Params:
;   Filename (Null terminated)
;   Destination location segment
;   Destination location offset
;   Disk number
;
; Returns:
;   al - Exit code : 0 - Success; 1 - File not found; 2 - Disk error
;
loadFile:
    ; Func prologue
    push bp
    mov bp, sp
    pusha

    mov byte [driveNum], dl
    mov bx, [bp+4]   ; dest Offset
    mov fs, [bp+6]   ; dest Segment
    mov si, [bp+8]   ; Filename

    mov cx, 10
    mov di, filename
.getFilename:
    lodsb           ; mov byte al, [si]; inc si
    cmp al, 0       ; End of filename
    dec si          ; Fill the rest with Null
    stosb
    loop .getFilename

    mov ax, FILETAB_LOC
    mov es, ax
    xor di, di      ; es:di - Filetable location
    mov si, filename
.filenameCheck:
    mov al, [di]
    cmp al, 0       ; End of filetable
    je .notFound

    cmp al, [si]
    je .beginCmp

    add di, 16
    jmp .filenameCheck
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
    mov si, filename
    pop di
    inc di
    jmp .filenameCheck
.notFound:
    mov byte [exitCode], NOT_FOUND
    jmp .done
.found:
    mov al, [es:di]
    inc di
    cmp al, 0       ; Skip NULLs untill extension
    je .found

    add di, 3       ; Skip extension
    inc di          ; Skip Number of entries

    xor ax, ax      ; Reset disk
    mov dl, [driveNum]
    int 0x13

    push fs
    mov ch, 0       ; Cylinder
    mov dh, 0       ; Head
    mov cl, [es:di] ; Sector
    inc di
    mov al, [es:di] ; Length in sectors
    mov ah, 2       ; Read from disk
    pop es          ; es:bx - Destination
    int 0x13

    jnc .done

    mov byte [exitCode], DISK_ERROR
.done:
    ; Func epilogue
    popa
    mov sp, bp
    pop bp
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
    pusha

    mov byte [driveNum], dl
    pop bx          ; src Offset
    pop fs          ; src Segment
    pop cx          ; Filesize 
    pop ax          ; Filetype

    push bx

    mov byte [filesize], cl

    mov di, filetype
    mov cx, 3
.filetype:
    lodsb
    cmp al, 0       ; End of filename
    dec si          ; Fill the rest with Null
    stosb
    loop .getFilename

    pop si          ; Filename
    mov cx, 10
    mov di, filename
.getFilename:
    lodsb           ; mov byte al, [si]; inc si
    cmp al, 0       ; End of filename
    dec si          ; Fill the rest with Null
    stosb
    loop .getFilename

    mov ax, FILETAB_LOC
    mov es, ax
    xor di, di      ; es:di - Filetable location
    mov si, filename
.filenameCheck:
    mov byte al, [es:di+14]
    mov byte [availableSector], al
    mov byte al, [es:di+15]
    add byte [availableSector], al

    mov al, [di]
    cmp al, 0       ; End of filetable
    je createFile   ; File doesn't exist, create new one

    cmp al, [si]
    je .beginCmp

    add di, 16
    jmp .filenameCheck
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
    mov si, filename
    pop di
    add di, 16
    jmp .filenameCheck
.found:             ; File already exists (override)
    ; TODO: Ask user if they want to override file
    mov al, [es:di]
    inc di
    cmp al, 0       ; Skip NULLs untill extension
    je .found

    add di, 3       ; Skip extension
    inc di          ; Skip Number of entries

    xor ax, ax      ; Reset disk
    mov dl, [driveNum]
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
    jmp return           ; TODO: Fix this

createFile:             ; Appends a file to the filetable.
    mov cx, 10
    mov si, filename
    rep movsb           ; Append the filename
    mov cx, 3
    mov si, filetype
    rep movsb           ; Append the extension
    mov byte [es:di], 0 ; Number of entries
    mov byte al, [availableSector]
    stosb               ; Starting sector
    mov byte al, [filesize]
    stosb               ; File length in sectors

    ; Write changed filetable to disk
    ;! If filetable size changes, this will have to change (probably)
    xor bx, bx          ; es:bx - Filetable location
    mov dl, [driveNum]
    mov ah, 3           ; Write to disk
    mov al, 1           ; Write 1 sector
    mov ch, 0           ; Cylinder
    mov dh, 0           ; Head
    mov cl, 1           ; Sector
    int 0x13

    jnc writeFileData

    mov byte [exitCode], DISK_ERROR
    jmp returnToCaller

writeFileData:          ; Writes file data to the disk
    mov ax, fs          ; Source segment
    mov es, ax
    pop bx              ; es:bx - Source
    mov ah, 3           ; Write to disk
    mov byte al, [filesize]
    mov dl, [driveNum]
    mov ch, 0           ; Cylinder
    mov dh, 0           ; Head
    mov byte cl, [availableSector]
    int 0x13

    jnc returnToCaller

    mov byte [exitCode], DISK_ERROR  ; Exit code - Disk Error

returnToCaller:
    ; Func epilogue
    popa
    mov sp, bp
    pop bp
    mov byte al, [exitCode]
    ret

;; Variables
availableSector: db 0
exitCode: db 0

driveNum: db 0
filesize: db 0
filetype: times 3 db 0
filename: times 10 db 0

;; Constants
FILETAB_LOC equ 0x1000

; Exit codes
SUCCESS equ 0
NOT_FOUND equ 1
DISK_ERROR equ 2
