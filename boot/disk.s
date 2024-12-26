; Reads sectors from disk
; Args:
;   eax - LBA low 32 bits
;   ebp - LBA high 32 bits
;   dl  - Drive number
;   es  - Destination segment
;   bx  - Destination offset
;   ecx - Byte count
; Rets:
;   CF  - Set on error
read_sectors:
    pusha

    ; Fill out Disk Address Packet
    mov si, .da_packet
    mov word [si],   0x10   ; Packet Size and Unused
    mov word [si+2], 1      ; Sector count
    mov word [si+4], bx     ; Dest. offset
    mov word [si+6], es     ; Dest. segment

    ; Store some regs for later
    push dx
    push si
    push eax
    push ebp

    ; Get bytes per sector
    mov ah, 0x48            ; Extended read drive parameters
    mov si, .drive_params   ; DS:SI - result buffer
    mov word [si], 0x1e     ; Buffer size
    int 0x13
    jc .done                ; CF set - error; we never pop ebp, eax, si & dx! (could cause issues)
    mov bp, word [si+24]    ; Bytes per sector

    ; Convert byte count in ECX to sector count in CX
    mov ax, cx  ; Low word
    shr ecx, 16 ; Shift high word into low word of ECX (therefore just CX)
    mov dx, cx  ; High word
    xor cx, cx  ; Clear CX
    div bp      ; Divide byte count in ax (low word) by bytes per sector
    test dx, dx ; DX will contain remainder
    setnz cl    ; (dx != 0) ? cl = 1 : cl = 0
    add cx, ax  ; Add remaining sectors to cx

    pop edx     ; Restore pushed EBP to EDX
    pop eax     ; Restore EAX
    pop si      ; Restore SI

    ; Calculate destination LBA
    div ebp                 ; Divide EAX by EBP
    mov dword [si+8],  eax  ; LBA low
    mov dword [si+12], 0    ; LBA high

    pop dx      ; Restore DX

; Read sectors one by one
.loop:
    mov ah, 0x42    ; Extended read
    clc             ; Clear CF
    int 0x13
    jc .done        ; Error

    add word [si+4], bp     ; Increment destination by sector byte count
    xor ebx, ebx            ; EBX = 0
    inc dword [si+8]        ; Increment LBA low
    seto bl                 ; Will set BL to 1 in case of overflow
    add dword [si+12], ebx  ; Increment LBA high

    loop .loop  ; dec cx; jnz .loop

.done:
    popa
    ret

; Disk Address Packet
.da_packet:     equ 0x8000

; Drive Parameters from int 13h, ah = 0x48
.drive_params:  equ 0x8010

