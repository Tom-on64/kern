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
section .text
global _start
extern kmain
_start:
	mov esp, __kernel_stack_top
	push eax
	push ebx
	call kmain		; Call kernel main

	; Hang if kernel returns
	cli
.hlt:	hlt
	jmp .hlt

