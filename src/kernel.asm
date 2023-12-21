[bits 16]

%define ENDL 0x0a, 0x0d

start:
    call resetTextMode

    ; Print start message
    mov si, startMsg
    call print

; Runs prompt
input:
    mov si, prompt
    call print
    mov di, userInput
    mov byte [inputLen], 0
.loop:
    xor ax, ax      ; Clear ax
    int 0x16        ; Get a character
    mov ah, 0x0e    ; Echo back input
    mov bh, 0
    int 0x10

    cmp al, 0x0d    ; Compare with CR
    je .enter

    mov [di], al    ; Add the input to userInput
    inc di
    jmp .loop
.enter:
    mov al, 0x0a    ; Print LF
    int 0x10
    mov byte [di], 0; NULL terminate userInput (Replaces CR)

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
    mov ax, userInput
    ; Get filetable into [es:di]
    mov bx, FILETAB_LOC
    mov es, bx
    xor bx, bx
    mov di, bx

    repne scasb     ; Scan for string at ax, in string at di
    je .found

    mov si, notFoundMsg
    call print

    jmp input
.found:

    jmp input

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
startMsg: db "kern.", ENDL, ENDL, 0 ; The bootup message
prompt: db "> ", 0 ; Prompt for input
newline: db ENDL, 0 ; CRLF
notFoundMsg: db "File not found!", ENDL, 0

browserMsg: db " Filename         Sector ", ENDL, \
               "----------       --------", ENDL, 0

helpMsg: db "Available Commands:", ENDL, \
            "F - List all files", ENDL, \
            "G - Graphics mode test", ENDL, \
            "H - Help", ENDL, \
            "R - Reboot", ENDL, 0

userInput: db 0
inputLen: db 0

FILETAB_LOC equ 0x1000

times 2*512-($-$$) db 0
