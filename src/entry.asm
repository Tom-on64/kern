[bits 32]
[global _start]
[global driveNum]

[extern main]

;; Calls the main function in 'main.c'
_start:
    pop ax
    mov [driveNum], al

    call main
    
    ; Exit QEMU
    mov ax, 0x2000
    mov dx, 0x0604
    out dx, ax

    jmp $       ;; If we used cli, hlt we wouldn't have interrupts :/

driveNum: db 0

