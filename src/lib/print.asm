;
; Prints a string
; si - Pointer to NULL terminated string
;
print:
    pusha
    mov ah, 0x0e
    mov bl, 0x1f
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

;
; Prints a hex byte
; dl - Hex byte
;
printHexByte:
    pusha
    xor cx, cx
    mov bh, 0
    mov bl, 0x1f    ; White on blue
.loop:
    mov al, dl
    and al, 0xF0
    ror al, 4
    add al, '0'
    cmp al, '9'
    jle .hexNum
    add al, 7
.hexNum:
    mov ah, 0x0e
    int 0x10
    inc cx
    cmp cx, 2
    je .done
    ror dl, 4
    jmp .loop
.done:
    popa
    ret

hexString: db "0x0000", 0

;
; Clears the screen in text mode
; Assumes VGA text 3 - 80x25, 16 color
;
clearText:
    pusha
    push es
    mov ax, 0x0b800
    mov es, ax
    xor di, di      ; es:di - b800:0000

    mov al, ' '     ; Fill with spaces
    mov ah, 0x17    ; White on blue
    mov cx, 80*25 
    rep stosw 

    mov ah, 2
    xor dx, dx      ; Set cursor to 0,0
    int 0x10

    pop es
    popa
    ret

;
; Writes a string into video memory
; di - Video memory offset
; si - string
;
drawString:
    pusha
    push es         ; For some reason doesn't get stored in pusha
    mov ax, VIDMEM
    mov es, ax
.loop:
    cmp byte [si], 0
    je .after
    movsb
    mov byte [es:di], 0x1f  ; White on blue
    inc di
    jmp .loop
.after:
    pop es
    popa
    ret

; Usefull definitions
%define ENDL 0x0a, 0x0d
VIDMEM equ 0x0b800
newline: db ENDL, 0 ; CRLF
