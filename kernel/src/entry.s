bits 32

; Flag bits
%define ALIGN	1<<0
%define MEMINFO	1<<1

; Field values
%define MAGIC	0x1BADB002
%define FLAGS	(ALIGN | MEMINFO)
%define CHECK	-(MAGIC + FLAGS)

; Multiboot 1 header
section .multiboot
align 4
multiboot:
	dd MAGIC
	dd FLAGS
	dd CHECK

; Initialize stack
section .bss
align 16
global __kernel_stack_top
global __kernel_stack_bottom
__kernel_stack_bottom:
	resb 16384
__kernel_stack_top:

; Entry point
section .boot
global _start
_start:	
    ; init temporary paging
    mov ecx, (init_pagedir - 0xC0000000)
    mov cr3, ecx

    mov ecx, cr4
    or ecx, 0x10
    mov cr4, ecx

    mov ecx, cr0
    or ecx, 0x80000000
    mov cr0, ecx

    jmp higher_half

section .text
extern kmain
higher_half:
	mov esp, __kernel_stack_top
	add ebx, 0xC0000000	; Make it a virtual address
	push eax 		; Magic
	push ebx		; Multiboot info	
	call kmain		; Call kernel main

	; Hang if kernel returns
	cli
.hlt:	hlt
	jmp .hlt

section .data
align 4096
global init_pagedir 
init_pagedir:
	dd 0b10000011
	times 768-1 dd 0	
	dd (0 << 22) | 0b10000011
	dd (1 << 22) | 0b10000011
	dd (2 << 22) | 0b10000011
	dd (3 << 22) | 0b10000011
	times 256-4 dd 0

