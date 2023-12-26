[bits 16]

launch:
    pusha
    call clearText

    mov di, 0x0e60      ; Offset in vidmem (2nd to last line)
    mov si, statusBarMsg
    call drawString
    mov di, 0x0f00      ; Last line
    mov si, controlsMsg
    call drawString

    mov di, hexCode
    xor cx, cx      ; Clear counter
getHexCh:
    xor ax, ax
    int 0x16
    cmp al, '$'     ; Check for end of input (Denoted by a "$")
    je .runInput
    cmp al, '!'     ; End editor, Exit back to kernel
    je return
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
    cmp di, hexCode+256
    jge getHexCh    ; Max length reached
    rol byte [hexByte], 4
    or byte [hexByte], al
    mov al, [hexByte]
    stosb
    mov dx, [hexByte]
    call printHexByte
    inc byte [codeLen]
    xor cx, cx
    mov al, ' '
    mov ah, 0x0e
    int 0x10
    jmp getHexCh
.backspace:
    mov byte [hexByte], 0
    dec byte [codeLen]
    xor cx, cx
    cmp di, hexByte     ; Check if there's anything to delete
    je getHexCh         ; If not, ignore
    mov byte [di], 0
    dec di
    mov si, backChars
    call print
    jmp getHexCh
.runInput:
    mov si, runMsg
    call print

    pusha
    call hexCode       ; Call the code
    popa

    mov si, ranMsg
    call print
    xor ax, ax
    int 0x16
    jmp launch

; Expects al to contain an ASCII character, will set bx to it's hex equivalent
asciiToHex:
    cmp al, '9'
    jle .getHex
    sub al, 7
.getHex:
    sub al, '0'
    ret

return:
    mov cx, 256
    mov di, hexCode
.loop:
    mov byte [di], 0
    inc di
    dec cx
    jnz .loop

    popa

    mov ax, 0x2000
    mov es, bx
    xor bx, bx

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    jmp 0x2000:0x0000

; - Includes -
%include "./src/lib/print.asm"
; ------------

statusBarMsg: db " -- HEX -- ", 0
controlsMsg: db "'$' to execute code; '!' to quit", 0
runMsg: db ENDL, ENDL, 0
ranMsg: db ENDL, ENDL, " - Finished Execution - ", ENDL, 0

backChars: db 8,8,8,'  ',8,8,0

hexByte: db 0

codeLen: db 0
hexCode: times 256 db 0     ; Code buffer
    ret

times 2*512-($-$$) db 0
