[bits 32]
[global _start]

[extern main]
[extern idtp]
[extern faultHandler]
[extern irqHandler]

;; Calls the main function in 'main.c'
_start:
    pop ax
    mov [driveNum], al
    call main
    jmp $       ;; If we used cli, hlt we wouldn't have interrupts :/

[global driveNum]
driveNum: db 0

;;
;; --- IDT ---
;;
[global idtLoad]
idtLoad:
    lidt [idtp]
    ret

;; Isrs (exceptions)
[global isr0]
isr0:
    cli
    push 0
    push 0
    jmp isrCommon
[global isr1]
isr1:
    cli
    push 0
    push 1
    jmp isrCommon
[global isr2]
isr2:
    cli
    push 0
    push 2
    jmp isrCommon
[global isr3]
isr3:
    cli
    push 0
    push 3
    jmp isrCommon
[global isr4]
isr4:
    cli
    push 0
    push 4
    jmp isrCommon
[global isr5]
isr5:
    cli
    push 0
    push 5
    jmp isrCommon
[global isr6]
isr6:
    cli
    push 0
    push 6
    jmp isrCommon
[global isr7]
isr7:
    cli
    push 0
    push 7
    jmp isrCommon
[global isr8]
isr8:
    cli
    push 8
    jmp isrCommon
[global isr9]
isr9:
    cli
    push 0
    push 9
    jmp isrCommon
[global isr10]
isr10:
    cli
    push 10
    jmp isrCommon
[global isr11]
isr11:
    cli
    push 11
    jmp isrCommon
[global isr12]
isr12:
    cli
    push 12
    jmp isrCommon
[global isr13]
isr13:
    cli
    push 13
    jmp isrCommon
[global isr14]
isr14:
    cli
    push 14
    jmp isrCommon
[global isr15]
isr15:
    cli
    push 0
    push 15
    jmp isrCommon
[global isr16]
isr16:
    cli
    push 0
    push 16
    jmp isrCommon
[global isr17]
isr17:
    cli
    push 0
    push 17
    jmp isrCommon
[global isr18]
isr18:
    cli
    push 0
    push 18
    jmp isrCommon
[global isr19]
isr19:
    cli
    push 0
    push 19
    jmp isrCommon
[global isr20]
isr20:
    cli
    push 0
    push 20
    jmp isrCommon
[global isr21]
isr21:
    cli
    push 0
    push 21
    jmp isrCommon
[global isr22]
isr22:
    cli
    push 0
    push 22
    jmp isrCommon
[global isr23]
isr23:
    cli
    push 0
    push 23
    jmp isrCommon
[global isr24]
isr24:
    cli
    push 0
    push 24
    jmp isrCommon
[global isr25]
isr25:
    cli
    push 0
    push 25
    jmp isrCommon
[global isr26]
isr26:
    cli
    push 0
    push 26
    jmp isrCommon
[global isr27]
isr27:
    cli
    push 0
    push 27
    jmp isrCommon
[global isr28]
isr28:
    cli
    push 0
    push 28
    jmp isrCommon
[global isr29]
isr29:
    cli
    push 0
    push 29
    jmp isrCommon
[global isr30]
isr30:
    cli
    push 0
    push 30
    jmp isrCommon
[global isr31]
isr31:
    cli
    push 0
    push 31
    jmp isrCommon

isrCommon:      ; No clue how this works, i'm pretty sure it calls the C function and has to push the registers or something idk
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, faultHandler
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret

;; IRQs
[global irq0]
irq0:
    cli
    push 0
    push 32
    jmp irqCommon
[global irq1]
irq1:
    cli
    push 0
    push 33
    jmp irqCommon
[global irq2]
irq2:
    cli
    push 0
    push 34
    jmp irqCommon
[global irq3]
irq3:
    cli
    push 0
    push 35
    jmp irqCommon
[global irq4]
irq4:
    cli
    push 0
    push 36
    jmp irqCommon
[global irq5]
irq5:
    cli
    push 0
    push 37
    jmp irqCommon
[global irq6]
irq6:
    cli
    push 0
    push 38
    jmp irqCommon
[global irq7]
irq7:
    cli
    push 0
    push 39
    jmp irqCommon
[global irq8]
irq8:
    cli
    push 0
    push 40
    jmp irqCommon
[global irq9]
irq9:
    cli
    push 0
    push 41
    jmp irqCommon
[global irq10]
irq10:
    cli
    push 0
    push 42
    jmp irqCommon
[global irq11]
irq11:
    cli
    push 0
    push 43
    jmp irqCommon
[global irq12]
irq12:
    cli
    push 0
    push 44
    jmp irqCommon
[global irq13]
irq13:
    cli
    push 0
    push 45
    jmp irqCommon
[global irq14]
irq14:
    cli
    push 0
    push 46
    jmp irqCommon
[global irq15]
irq15:
    cli
    push 0
    push 47
    jmp irqCommon

irqCommon: ; Simular to isrCommon, probaly calls the C function
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, irqHandler
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret

