[bits 16]

launch:
    call resetTextMode
    mov si, headerMsg
    call print
    xor cx, cx
    mov di, hexCode

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
    mov [di], al
    mov dx, [hexByte]
    call printHexByte
    inc di
    xor cx, cx
    mov al, ' '
    mov ah, 0x0e
    int 0x10
    jmp getHexCh
.backspace:
    mov byte [hexByte], 0
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
    call hexBegin       ; Call the code
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
    mov ax, 0x2000
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x2000:0x0000

; - Includes -
%include "./src/lib/screen.asm"
%include "./src/lib/print.asm"
; ------------

headerMsg: db   " - EDITOR - ", ENDL, \
                "Type '$' to run your code or '!' to exit", ENDL, ENDL, 0
runMsg: db ENDL, " -- Executing -- ", ENDL, ENDL, 0

backChars: db 8, 8, 8, '  ', 8, 8, 0

hexByte: db 0

hexBegin: ; The label we jump to
    pusha
hexCode: times 256 db 0     ; Code buffer (user input code)
hexEnd: 
    popa
    ret

times 2*512-($-$$) db 0
