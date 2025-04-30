; ISR Assembly
[bits 32]

isr_common:
	;; Push registers to the int_frame_t struct
	
	pushad
	;; push eax
	;; push ecx
	;; push edx
	;; push ebx
	;; push esp
	;; push ebp
	;; push esi
	;; push edi
	push ds
	push es
	push fs
	push gs

	;; Load kernel data segment
	push ebx
	mov bx, 0x10
	mov ds, bx
	mov es, bx
	mov fs, bx
	mov gs, bx
	pop ebx
	
	[extern isr_handle_interrupt]
	call isr_handle_interrupt
[global isr_return]
isr_return:	; Also used by task switching
	pop gs
	pop fs
	pop es
	pop ds
	popad
	add esp, 8 ; Pop interupt number and error code
	iretd ; Pop CS, EIP & EFLAGS (Also SS & ESP if it's from userspace)

%macro ISR_WITH_ERRNO 1
isr%1:
	push dword %1
	jmp isr_common
%endmacro
	
%macro ISR_WITHOUT_ERRNO 1
isr%1:
	push dword 0	; Instead of error code
	push dword %1
	jmp isr_common
%endmacro

;; Exceptions
ISR_WITHOUT_ERRNO	0
ISR_WITHOUT_ERRNO	1
ISR_WITHOUT_ERRNO	2
ISR_WITHOUT_ERRNO	3
ISR_WITHOUT_ERRNO	4
ISR_WITHOUT_ERRNO	5
ISR_WITHOUT_ERRNO	6
ISR_WITHOUT_ERRNO	7
ISR_WITH_ERRNO		8
ISR_WITHOUT_ERRNO	9
ISR_WITH_ERRNO		10
ISR_WITH_ERRNO		11
ISR_WITH_ERRNO		12
ISR_WITH_ERRNO		13
ISR_WITH_ERRNO		14
ISR_WITHOUT_ERRNO 	15
ISR_WITHOUT_ERRNO 	16
ISR_WITHOUT_ERRNO	17
ISR_WITHOUT_ERRNO	18
ISR_WITHOUT_ERRNO	19
ISR_WITHOUT_ERRNO	20
ISR_WITHOUT_ERRNO	21
ISR_WITHOUT_ERRNO	22
ISR_WITHOUT_ERRNO	23
ISR_WITHOUT_ERRNO	24
ISR_WITHOUT_ERRNO	25
ISR_WITHOUT_ERRNO	26
ISR_WITHOUT_ERRNO	27
ISR_WITHOUT_ERRNO	28
ISR_WITHOUT_ERRNO	29
ISR_WITHOUT_ERRNO	30
ISR_WITHOUT_ERRNO	31

;; IRQs 0-15 (mapped 32-47)
ISR_WITHOUT_ERRNO	32
ISR_WITHOUT_ERRNO	33
ISR_WITHOUT_ERRNO	34
ISR_WITHOUT_ERRNO	35
ISR_WITHOUT_ERRNO	36
ISR_WITHOUT_ERRNO	37
ISR_WITHOUT_ERRNO	38
ISR_WITHOUT_ERRNO	39
ISR_WITHOUT_ERRNO	40
ISR_WITHOUT_ERRNO	41
ISR_WITHOUT_ERRNO	42
ISR_WITHOUT_ERRNO	43
ISR_WITHOUT_ERRNO	44
ISR_WITHOUT_ERRNO	45
ISR_WITHOUT_ERRNO	46
ISR_WITHOUT_ERRNO	47

;; Syscall ISR128
[global isr128]
isr128:
	cli	; Prevent race condition & other issues
	push dword 0
	push dword 128
	jmp isr_common

;; Create table of ISRs
[global isr_redirect_table]
isr_redirect_table:
%assign i 0
%rep 48
	dd isr%+i
%assign i i+1
%endrep

