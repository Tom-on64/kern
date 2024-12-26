; Global Descriptor Table
gdt:
.null_desc: ; Just zeros, no clue why it even exists
    dq 0
.code_desc: ; 32-bit code
    dw 0xffff       ; Limit (low 16-bits)
    dw 0x0000       ; Base (low 16 bits)
    db 0x00         ; Base (mid 8 bits)
    db 0b10011011   ; Access flags
    db 0b11001111   ; Low 4 bits - Granularity, High 4 bits - Limit (high 4 bits)
    db 0x00         ; Base (high 8 bits)
.data_desc: ; 32-bit data
    dw 0xffff       ; Limit (low 16-bits)
    dw 0x0000       ; Base (low 16 bits)
    db 0x00         ; Base (mid 8 bits)
    db 0b10010011   ; Access flags
    db 0b11001111   ; Low 4 bits - Granularity, High 4 bits - Limit (high 4 bits)
    db 0x00         ; Base (high 8 bits)
.end:

; GDT Pointer
gdtp:
    dw gdt.end - gdt - 1    ; Size
    dd gdt                  ; Pointer

; Segments
code_seg equ gdt.code_desc - gdt
data_seg equ gdt.data_desc - gdt

