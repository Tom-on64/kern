[bits 16]

start:
    pusha
    mov byte [driveNum], dl

hexEditor:
    call clearText

    mov di, 0x0e60      ; Offset in vidmem (2nd to last line)
    mov si, hexModeMsg
    call drawString
    mov di, 0x0f00      ; Last line
    mov si, controlsMsg
    call drawString

    mov di, hexCode
    xor cx, cx      ; Clear counter

    mov dx, [codeLen]
    call printHexByte
    mov si, bytesMsg
    call print
    mov si, newline
    call print

    cmp word [codeLen], 0
    je getHexCh

    mov cx, [codeLen]
.printCode:  
    mov dl, [di]
    call printHexByte
    mov ah, 0x0e
    mov al, ' '
    int 0x10
    inc di
    dec cx
    jnz .printCode

    mov cx, [codeLen]
getHexCh:
    xor ax, ax
    int 0x16
    cmp al, RUN_CODE
    je .runInput
    cmp al, QUIT_EDIT
    je return
    cmp al, SAVE_FILE
    je save
    cmp al, OPEN_FILE
    je open
    cmp al, 0x08    ; Backspace
    je .backspace

    xor ah, ah
    call asciiToHex ; Get the hex digit into bx

    cmp ax, 15      ; Check if it's a hex character
    jg getHexCh     ; If not, ignore it

    inc cx
    cmp cx, 2       ; Check if we've gotten a whole byte (2 hex digits/nibbles)
    je .putByte
    mov [hexByte], al
    jmp getHexCh
.putByte:
    cmp di, hexCode+510
    jge getHexCh    ; Max length reached
    rol byte [hexByte], 4
    or byte [hexByte], al
    mov al, [hexByte]
    stosb
    mov dx, [hexByte]
    call printHexByte
    inc word [codeLen]
    xor cx, cx
    mov al, ' '
    mov ah, 0x0e
    int 0x10
    jmp getHexCh
.backspace:
    cmp word [codeLen], 0 ; Check if there's anything to delete
    je getHexCh         ; If not, ignore
    mov byte [hexByte], 0
    dec word [codeLen]
    xor cx, cx
    mov byte [di], 0
    dec di
    mov si, hexBackspace
    call print
    jmp getHexCh
.runInput:
    mov si, runMsg
    call print

    pusha
    push es
    call hexCode       ;! Runs arbitrary code (may break everything)
    pop es
    popa

    mov si, ranMsg
    call print
    xor ax, ax
    int 0x16
    jmp hexEditor
return:
    popa

    mov ax, 0x2000
    mov es, ax
    xor bx, bx

    mov ds, ax
    mov fs, ax
    mov gs, ax
    jmp 0x2000:0x0000

; Saves a file
save:
    mov dl, 9
    mov si, saveFileMsg
    call getInput
    mov byte al, [filename]
    cmp byte al, 0
    je getHexCh

    ; Call saveFile
    mov byte dl, [driveNum]
    push word filename
    push word binFile   ; Filetype, TODO: More filetypes other than .bin
    push word 1         ; TODO: Dynamic filesize
    push word 0x8000    ; Program location (this segment)
    push word savedData ; Offset (hex code location)
    call saveFile       ; Returns exit code in al
    add sp, 10          ; Stack bs

    mov ax, 0x8000
    mov es, ax
    mov fs, ax

    cmp al, 0           ; Won't be 1 (File not found) so must be disk error
    jne .error

    jmp getHexCh
.error:
    mov si, diskErrMsg
    mov di, 0x0f00      ; Last line
    call drawString
    
    jmp getHexCh

; Opens a file
open:
    mov dl, 11
    mov si, openFileMsg
    call getInput
    mov byte al, [filename]
    cmp byte al, 0
    je getHexCh

    ; Call loadFile
    mov byte dl, [driveNum]
    push word filename
    push word 0x8000
    push word hexCode
    call loadFile

    mov ax, 0x8000
    mov es, ax
    mov fs, ax

    cmp al, 1
    je .notFound
    cmp al, 2
    je .diskErr

    ; Success
    jmp hexEditor
.notFound:
    mov si, notFoundMsg
    jmp .err
.diskErr:
    mov si, diskErrMsg
.err:
    mov di, 0x0f00
    call drawString
    xor ax, ax
    int 0x16
    jmp hexEditor

;
; Gets a filename (10 chars, If less, padded with Nulls)
; Outputs the filename in the filename buffer
; si - Prompt string (Null terminated)
; dl - String offset
;
getInput:
    pusha
    push es

    mov di, 0x0f00      ; Last line
    call drawString     ; si should contain the string

    mov ah, 2       ; Move cursor to input position
    mov dh, 24      ; Last line
    int 0x10

    mov di, filename
    xor cx, cx      ; Entered filename length
.loop:
    xor ax, ax
    int 0x16

    cmp al, 0xD     ; User pressed enter
    je .done
    cmp al, 8       ; Backspace
    je .backspace
    cmp al, 27      ; Escape
    je .escape
    cmp cx, 10      ; Filename too long
    je .loop

    mov ah, 0x0e
    int 0x10
    mov [di], al
    inc di
    inc cx
    jmp .loop
.backspace:
    cmp cx, 0
    je .loop
    dec cx
    dec di
    mov si, textBackspace
    call print
    jmp .loop
.escape:
    xor cx, cx
    mov si, filename
.done:
    cmp cx, 10
    je .after
    mov byte [di], 0
    inc cx
    inc di
.after:
    mov ah, 2       ; Move cursor
    xor dx, dx      ; 0,0
    int 0x10

    pop es
    popa
    ret

; Expects al to contain an ASCII character, will set bx to it's hex equivalent
asciiToHex:
    cmp al, '9'
    jle .getHex
    sub al, 7
.getHex:
    sub al, '0'
    ret

;; Includes
%include "./src/lib/print.asm"
%include "./src/lib/disk.asm"

;; Variables
hexModeMsg: db " -- HEX -- ", 0
textModeMsg: db " -- Text -- ", 0
controlsMsg: db "'$' Execute; ^Q Quit; ^S Save; ^O Open", 0
saveFileMsg: db "Save as:                              ", 0 ; Make sure to erase the controls
openFileMsg: db "Open file:                            ", 0
bytesMsg: db " Bytes", 0
; savedMsg: db "Saved file as '", 0
; oepenedMsg: db "Editing file '", 0
runMsg: db ENDL, ENDL, 0
ranMsg: db ENDL, ENDL, " - Finished Execution - ", ENDL, 0
notFoundMsg: db "File not found!           ", 0 ; Erase the user input
diskErrMsg: db  "Disk Error!               ", 0

; Filetypes
binFile: db "bin"
txtFile: db "txt"

hexBackspace: db 8,8,8,'  ',8,8,0
textBackspace: db 8,' ',8,0

driveNum: db 0
hexByte: db 0

filenameLen: db 0
filename: times 10 db 0

;; Constants
RUN_CODE equ '$'
QUIT_EDIT equ 'q'
NEW_FILE equ 'n'
SAVE_FILE equ 's'
OPEN_FILE equ 'o'

times 3*512-($-$$) db 0

savedData:
    hexCode: times 510 db 0     ; Code buffer
    codeLen: dw 0               ; Hopefully we don't overflow :)
