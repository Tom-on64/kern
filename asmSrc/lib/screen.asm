resetTextMode: 
    pusha

    mov ah, 0       ; Set mode
    mov al, 0x03    ; 25x80, Text mode
    int 0x10

    mov ah, 0x0b    ; Change color/palette
    mov bh, 0x00    ; Change BG color
    mov bl, 0x01    ; Blue BG
    int 0x10

    popa
    ret

resetGraphicsMode:
    pusha

    mov ah, 0       ; Set mode
    mov al, 0x13    ; 320x200, 256 color graphics mode
    int 0x10

    popa
    ret
