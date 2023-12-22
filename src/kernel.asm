[bits 16]

start:
    ; Save the drive number from stack
    mov byte [driveNumber], dl

    call resetTextMode

    ; Print start message
    mov si, startMsg
    call print

; Runs prompt
input:
    mov si, prompt
    call print
    mov di, userInput
    xor cx, cx
.loop:
    xor ax, ax      ; Clear ax
    int 0x16        ; Get a character
    mov ah, 0x0e    ; Echo back input
    mov bh, 0
    int 0x10

    cmp al, 0x0d    ; Compare with CR
    je .enter

    mov [di], al    ; Add the input to userInput
    inc cl
    inc di
    jmp .loop
.enter:
    mov al, 0x0a    ; Print LF
    int 0x10
    mov byte [di], 0 ; NULL terminate userInput (Replaces CR)
    mov byte [inputLen], cl

    mov al, [userInput]
    cmp al, 'H'
    je help
    cmp al, 'F'
    je fileBrowser
    cmp al, 'G'
    je graphicsTest
    cmp al, 'R'
    je reboot
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
    cmp al, '}'     ; Check for end of filetable
    je .notFound

    cmp al, [di]    ; Check for starting character
    je .beginSearch

    inc bx
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
    cmp al, ':'     ; Check if we are at the end of a filename
    je executeProgram
.restartSearch:
    mov di, userInput
    pop bx
    inc bx
    jmp .nextCh
.notFound:
    mov si, fileNotFoundMsg
    call print
    jmp input
    
; Expects [es:bx] to point to a sector number folowed by a comma ('XX,' where X is 0-9)
executeProgram:
    mov cl, 10      ; Get 10 to multiply with
    xor al, al      ; Reset al
    inc bx          ; Skip ':'
.getSectorNum:
    mov dl, [es:bx]
    inc bx
    cmp dl, ','     ; Check if its end of sector number
    je .load
    cmp dl, '0'     ; Check if it is a digit 0-9
    jl .notFound
    cmp dl, '9'
    jg .notFound
    sub dl, '0'     ; Convert ASCII char to an integer
    mul cl          ; ax *= cl (al *= 10)
    add al, dl      ; al += dl
    jmp .getSectorNum
.notFound:
    mov si, sectorNotFoundMsg
    call print
    jmp input
.load:
    mov cl, al      ; Sector

    mov ah, 0       ; Reset disk system
    mov dl, byte [driveNumber]
    int 0x13

    mov bx, PROGRAM_LOC
    mov es, bx
    xor bx, bx      ; [es:bx] - Place to load out program

    mov ah, 2       ; Read sectors into memory
    mov dl, byte [driveNumber]
    mov al, 1       ; Read 1 sector
    mov ch, 0       ; Cylinder
    mov dh, 0       ; Head
    int 0x13
    jnc .loaded     ; Loaded file successfully

    mov si, programLoadFailMsg
    call print

    jmp input
.loaded:
    mov ax, PROGRAM_LOC
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp PROGRAM_LOC:0x0000

;
; Help - Prints the help message (helpMsg)
;
help:
    mov si, helpMsg
    call print
    jmp input

;
; File Browser
;
fileBrowser:
    mov si, browserMsg
    call print

; Prints the filetable
printFiletable:
    mov bx, FILETAB_LOC
    mov es, bx
    xor bx, bx
    xor cx, cx
.filetableLoop:
    inc bx
    mov al, [es:bx] ; Get the filetable character
    cmp al, '}'     ; Check for filetable end
    je .done
    cmp al, ':'     ; Check for sector number
    je .sectorLoop
    cmp al, ','     ; Check for another entry
    je .entryLoop
    inc cx
    int 0x10
    jmp .filetableLoop
.sectorLoop:
    cmp cx, 20
    je .filetableLoop
    mov al, ' '
    int 0x10
    inc cx
    jmp .sectorLoop
.entryLoop:
    xor cx, cx
    mov si, newline
    call print
    jmp .filetableLoop
.done:
    mov si, newline
    call print
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
%include "./src/print.asm"
%include "./src/screen.asm"

; --------
;   Data
; --------
driveNumber: db 0

startMsg: db "kern.", ENDL, ENDL, 0 ; The bootup message
prompt: db "> ", 0 ; Prompt for input
fileNotFoundMsg: db "File not found!", ENDL, 0 ; User tries to load nonexistent file
sectorNotFoundMsg: db "Sector not found!", ENDL, 0 ; File in an incorrect sector in filetable
programLoadFailMsg: db "Failed while loading program!", ENDL, 0 ; File load fail

browserMsg: db " Filename         Sector ", ENDL, \
               "----------       --------", ENDL, 0

helpMsg: db "Available Commands:", ENDL, \
            "F - List all files", ENDL, \
            "G - Graphics mode test", ENDL, \
            "H - Help", ENDL, \
            "R - Reboot", ENDL, 0

inputLen: db 0
userInput: db 0

FILETAB_LOC equ 0x1000
PROGRAM_LOC equ 0x8000

times 2*512-($-$$) db 0
