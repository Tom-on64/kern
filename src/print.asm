;
; Prints a string
; si - Pointer to NULL terminated string
;
print:
    pusha
    mov ah, 0x0e
    mov bl, 0x0f
    mov bh, 0
.loop:
    lodsb           ; Load [si] into al; Increment si
    or al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    popa
    ret

;
; Prints a hex value
; dx - Value to print in Hex
;
printHex:
    pusha
    xor cx, cx      ; Init counter
.loop:
    cmp cx, 4       ; Check for loop end
    je .done
    mov ax, dx      ; Copy dx to ax
    and ax, 0x000f  ; Get last nibble (4 bits) of ax
    add ax, '0'     ; Convert number to an ASCII number (0-9)
    cmp ax, '9'     ; Check if number was less than 9, meaning we can use it
    jle .addChar
    add ax, 0x07    ; Convert to letter (A-F)
.addChar:
    mov bx, hexString + 5
    sub bx, cx      ; Get position of current hex character
    mov [bx], al
    ror dx, 4       ; Rotate bits of dx (0x1a2b -> 0xb1a2)
    inc cx          ; Increment the counter
    jmp .loop
.done: 
    mov si, hexString
    call print
    popa
    ret

hexString: db "0x0000", 0

; Usefull definitions
%define ENDL 0x0a, 0x0d
newline: db ENDL, 0 ; CRLF
