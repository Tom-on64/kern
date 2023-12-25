[bits 16]

start:
    ; Save the drive number from stack
    mov byte [driveNumber], dl

    mov bx, 0x8000
    mov es, bx
    xor bx, bx

    call resetTextMode

    ; Print start message
    mov si, startMsg
    call print

; Runs prompt
input:
    mov si, prompt
    call print
    mov ax, 0x2000
    mov ds, ax
    mov es, ax
    mov si, userInput
    xor cx, cx
.loop:
    xor ax, ax      ; Clear ax
    int 0x16        ; Get a character
    mov ah, 0x0e    ; Echo back input
    mov bh, 0
    int 0x10

    cmp al, 0x0d    ; Compare with CR
    je .enter

    mov [si], al    ; Add the input to userInput
    inc cx
    inc si
    jmp .loop
.enter:
    mov al, 0x0a    ; Print LF
    int 0x10
    mov byte [si], 0 ; NULL terminate userInput (Replaces CR)
    mov byte [inputLen], cl

    cmp byte [inputLen], 0 ; Check if user input something
    je input

; Runs a command in userInput
runCommand:
    mov di, lsCmd
    call .cmp
    je fileList

    mov di, gfxCmd
    call .cmp
    je graphicsTest

    mov di, helpCmd
    call .cmp
    je help

    mov di, rebootCmd
    call .cmp
    je reboot

    mov di, clearCmd
    call .cmp
    je clearScreen

    jmp .end
.cmp:
    push cx
    mov si, userInput
    repe cmpsb
    pop cx
    ret
.end:

;
; Load Program - Takes userInput and tries to run that file
;
loadProgram:
    mov di, userInput
    mov bx, FILETAB_LOC
    mov es, bx
    xor bx, bx      ; Get [es:bx] to be start of filetable
.nextCh:
    mov al, [es:bx]
    cmp al, 0       ; Check for end of filetable
    je .notFound

    cmp al, [di]    ; Check for starting character
    je .beginSearch

    add bx, 16      ; Move to the next entry
    jmp .nextCh
.beginSearch:
    push bx         ; Save position, if it doesn't equal
    mov byte cl, [inputLen]
.searchLoop:
    mov al, [es:bx]
    inc bx
    cmp al, [di]
    jne .restartSearch

    dec cl
    jz .match
    inc di
    jmp .searchLoop
.match:
    mov al, [es:bx]
    cmp al, 0       ; Check if we are at the end of a filename
    je foundProgram
.restartSearch:
    mov di, userInput
    pop bx
    inc bx
    jmp .nextCh
.notFound:
    mov si, fileNotFoundMsg
    call print
    jmp input
    
foundProgram:
    mov al, [es:bx]
    cmp al, 0       ; Loop until we find the extension
    jne .getMetadata    
    inc bx
    jmp foundProgram
.getMetadata:
    mov al, [es:bx]
    mov [fileExt], al
    inc bx
    mov al, [es:bx]
    mov [fileExt+1], al
    inc bx
    mov al, [es:bx]
    mov [fileExt+2], al
    inc bx

    inc bx          ; Ignore Number of FT entries for now

    mov cl, [es:bx] ; Starting sector
    inc bx
    mov al, [es:bx] ; Number of sectors to read
    push ax         ; Save file size

    mov ah, 0       ; Reset disk system
    mov dl, byte [driveNumber]
    int 0x13

    mov bx, PROGRAM_LOC
    mov es, bx
    xor bx, bx      ; [es:bx] - Place to load out program

    mov ah, 2       ; Read sectors into memory
    mov dl, byte [driveNumber]
    mov ch, 0       ; Cylinder
    mov dh, 0       ; Head
    int 0x13
    jnc .loaded     ; Loaded file successfully

    mov si, programLoadFailMsg
    call print

    jmp input
.loaded:
    ; Check file extension. If 'bin', then run, if 'txt', print
    pop ax      ; Restore file size

    mov cx, 3   ; Check file extension
    mov si, fileExt 
    mov ax, 0x2000
    mov es, ax
    mov di, binExt
    repe cmpsb
    jne .printText  ; TODO: Check for more file extensions

    mov ax, PROGRAM_LOC
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp PROGRAM_LOC:0x0000
.printText:
    mov bx, PROGRAM_LOC
    mov es, bx
    xor bx, bx
    mov cx, 512
    mul cx
    mov cx, ax
    mov ah, 0x0e
.textLoop:
    mov al, [es:bx]
    cmp al, 0
    je .rep
    int 0x10
.rep:
    inc bx
    dec cx
    jnz .textLoop

    mov si, newline
    call print
    jmp input

;
; Help - Prints the help message (helpMsg)
;
help:
    mov si, helpMsg
    call print
    jmp input

;
; Clears the screen
;
clearScreen:
    call resetTextMode
    jmp input

;
; File list (ls)
;
fileList:
    mov si, fileListMsg
    call print

; Prints the filetable
printFiletable:
    mov bx, FILETAB_LOC
    mov es, bx
    xor bx, bx
    xor cx, cx      ; Entry byte couter
    mov ah, 0x0e    ; Prepare for printing
.filenameLoop:
    mov al, [es:bx] ; Get filename character
    cmp al, 0       ; Check if it's NULL
    je .filenameEnd ; If so, end the filename reading
    cmp cx, 9       ; Check if we already read 10 Bytes
    je .filenameEnd 
    int 0x10        ; Else print the character
    inc cx          ; Increment byte counter
    inc bx
    jmp .filenameLoop
.filenameEnd:
    push cx
.skipNull:
    cmp cx, 0
    je .done
    cmp cx, 10      ; Check if we're at the end of filename
    je .fileExt     ; If so, print extension
    inc cx
    inc bx
    jmp .skipNull
.fileExt:
    ; push cx         ; Save the counter
    mov al, '.'     ; Print a "." to separate filename from extension
    int 0x10
.fileExtLoop:
    mov al, [es:bx]
    cmp cx, 13      ; Check if we printed 3 characters
    je .fileExtEnd
    cmp al, 0
    je .fileExtEnd
    int 0x10
    inc cx
    inc bx
    jmp .fileExtLoop
.fileExtEnd:
    cmp cx, 13
    je .whitespace
    inc cx
    inc bx
    mov al, ' '     ; Print whitespace
    int 0x10
    jmp .fileExtEnd
.whitespace:
    pop cx
.wsLoop:
    cmp cx, 11      ; Max filename length + 1 for spacing
    je .fileInfo
    mov al, ' '
    int 0x10
    inc cx
    jmp .wsLoop
.fileInfo:
    ; Entry number
    mov dl, [es:bx]
    call printHexByte
    inc bx
    mov al, ' '
    int 0x10
    ; Start sector
    mov dl, [es:bx]
    call printHexByte
    inc bx
    mov al, ' '
    int 0x10
    int 0x10
    int 0x10
    ; File size
    mov al, [es:bx]
    inc bx
    cmp al, 1
    je .oneSector       ; Only print 512B
    shr al, 1           ; Divide by 2
    jc .oneHalf         ; We had an odd number of sectors
    add al, '0'
    int 0x10
    mov si, kilobyteText
    call print
    jmp .repeat
.oneHalf:
    add al, '0'
    int 0x10
    mov si, oneHalfText
    call print
    jmp .repeat
.oneSector:
    mov si, sectorLenText
    call print
.repeat:
    mov si, newline
    call print
    xor cx, cx
    jmp .filenameLoop
.done:
    jmp input

;
; Graphics Mode Test
;
graphicsTest:
    call resetGraphicsMode

    ; Test square
    mov ah, 0x0c        ; Write gfx pixel
    mov al, 0x02        ; Green
    mov bh, 0           ; Page number

    mov cx, 100         ; Column number
    mov dx, 100         ; Row number
    int 0x10
.colLoop:
    inc cx
    int 0x10
    cmp cx, 150
    jne .colLoop

    inc dx
    int 0x10
    mov cx, 99
    cmp dx, 150
    jne .colLoop
.done:
    mov si, startMsg
    call print

    xor ax, ax
    int 0x16

    call resetTextMode
    jmp input

;
; Reboots the OS
;
reboot: jmp 0xffff:0x0000

; ----------
;  Includes
; ----------
%include "./src/lib/print.asm"
%include "./src/lib/screen.asm"

; --------
;   Data
; --------
driveNumber: db 0

startMsg: db "kern.", ENDL, ENDL, 0 ; The bootup message
prompt: db "> ", 0 ; Prompt for input
fileNotFoundMsg: db "File not found!", ENDL, 0 ; User tries to load nonexistent file
programLoadFailMsg: db "Failed while loading program!", ENDL, 0 ; File load fail
sectorLenText: db "512B", 0
kilobyteText: db "kB", 0
oneHalfText: db ".5kB", 0

fileListMsg: db " Filename      E# St   Size ", ENDL, \
                "----------    ------- ------", ENDL, 0

helpMsg: db "Available Commands:", ENDL, \
            "clear - Clears the screen", ENDL, \
            "gfx - Graphics mode test", ENDL, \
            "help - Prints this message", ENDL, \
            "ls - List all files", ENDL, \
            "reboot - Reboots the system", ENDL, 0

; Commands
; TODO: Put these into a file or something
clearCmd: db "clear", 0
editCmd: db "edit", 0
gfxCmd: db "gfx", 0
helpCmd: db "help", 0
lsCmd: db "ls", 0
rebootCmd: db "reboot", 0

; Files
fileExt: db "   ", 0
binExt: db "bin", 0
txtExt: db "txt", 0

inputLen: db 0
userInput: db 0

FILETAB_LOC equ 0x1000
PROGRAM_LOC equ 0x8000

times 3*512-($-$$) db 0
