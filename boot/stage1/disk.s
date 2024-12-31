; Reads sectors from disk
; eax 	- LBA
; cx  	- Sector count
; dl  	- Drive number
; es:dx	- Destination
read_sectors:
	pusha

	; Fill out Disk Address Packet
	mov si, .da_packet
	mov word 	[si],   0x10	; Packet Size and Unused
	mov word 	[si+2], cx	; Sector count
	mov word 	[si+4], bx	; Dest. offset
	mov word 	[si+6], es	; Dest. segment
	mov dword	[si+8], eax	; LBA

	mov ah, 0x42    ; Extended read
	clc             ; Clear CF
	int 0x13
.done:
	popa
	ret

; Disk Address Packet
.da_packet:     equ 0x8000

