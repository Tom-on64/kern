[bits 16]

start:
    mov si, msg
    call print
    cli
    hlt

%include "./src/print.asm"

msg: db "Hello!", ENDL, 0

times 512-($-$$) db 0
