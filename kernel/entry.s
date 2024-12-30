;
; Kernel entry point
;

MBH_MAGIC	equ 0xe85250d6
MBH_ARCH	equ 0x00000000
STACK_SIZE	equ 4096
global VIRT_BASE
KERN_VIRT_BASE	equ 0xC000000
KERN_PAGE_NUM	equ (KERN_VIRT_BASE >> 22)

bits 32

; Multiboot2 Header
section .multiboot
align 64
mbh:
; Magic fields
.magic: 	dd MBH_MAGIC
.arch:		dd MBH_ARCH
.size:		dd (mbh.end - mbh)
.checksum:	dd 0x10000000 - (MBH_MAGIC + MBH_ARCH + (mbh.end - mbh))

; Framebuffer Tag
align 8
dw 5	; Type - Framebuffer
dw 1	; Flags	- Optional
dd 20	; Size
dd 1920	; Width
dd 1080	; Height
dd 32	; Depth

; Zero Tag
align 8
dw 0	; Type - Zero
dw 0	; Flags
dd 8	; Size	

.end:

; Stack
section .bss
align 16
stack_bottom:
	resb 16384*8
stack_top:

; Entry section
section .boot
global start

extern kmain
start:
	; Init temporary paging
	mov eax, (initpd - 0xC0000000)
	mov cr3, eax

	mov ecx, cr4
	or ecx, 0x10	; Paging enable bit
	mov cr4, ecx

	mov ecx, cr0
	or ecx, 0x8000000
	mov cr0, ecx

	jmp higher_half

Shutdown:
	mov ax, 0x1000
	mov ss, ax
	mov sp, 0xf000
	mov ax, 0x5307
	mov bx, 0x0001
	mov cx, 0x0003
	int 0x15

section .text
higher_half:
	mov esp, stack_top	; Stack Pointer

	add ebx, KERN_VIRT_BASE ; Make address in EBX virtual
	push ebx		; Push multiboot mem info pointer

	call kmain

; Halt on return
cli
h: 	hlt
	jmp h

section .data

; Inital Page Directory
align 4096
global initpd
initpd:
	dd 0b10000011		; Initial 4MB identity map

	times 768-1 dd 0	; Padding

	; Higher half kernel start, map 16MB
	dd (0 << 22) | 0b10000011
	dd (1 << 22) | 0b10000011
	dd (2 << 22) | 0b10000011
	dd (3 << 22) | 0b10000011

	times 256-4 dd 0	; More padding

