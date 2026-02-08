[bits 64]

[section .text]
isr_common:
	push rax
	push rbx
	push rcx
	push rdx
	push rsi
	push rdi
	push rbp
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	mov rbp, ds
	push rbp

	mov rdi, rsp

	[extern isr_handle_interrupt]
	call isr_handle_interrupt

[global isr_return]
isr_return:
	pop rbp
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rbp
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rbx
	pop rax

	add rsp, 16	; Pop errno and int number
	iretq

%macro ISR_WITH_ERRNO 1
[global isr%1]
isr%1:
	push %1
	jmp isr_common
%endmacro

%macro ISR_WITHOUT_ERRNO 1
[global isr%1]
isr%1:
	push 0
	push %1
	jmp isr_common
%endmacro

;; Exceptions
ISR_WITHOUT_ERRNO 0
ISR_WITHOUT_ERRNO 1
ISR_WITHOUT_ERRNO 2
ISR_WITHOUT_ERRNO 3
ISR_WITHOUT_ERRNO 4
ISR_WITHOUT_ERRNO 5
ISR_WITHOUT_ERRNO 6
ISR_WITHOUT_ERRNO 7
ISR_WITH_ERRNO    8
ISR_WITHOUT_ERRNO 9
ISR_WITH_ERRNO    10
ISR_WITH_ERRNO    11
ISR_WITH_ERRNO    12
ISR_WITH_ERRNO    13
ISR_WITH_ERRNO    14
ISR_WITHOUT_ERRNO 15
ISR_WITHOUT_ERRNO 16
ISR_WITHOUT_ERRNO 17
ISR_WITHOUT_ERRNO 18
ISR_WITHOUT_ERRNO 19
ISR_WITHOUT_ERRNO 20
ISR_WITHOUT_ERRNO 21
ISR_WITHOUT_ERRNO 22
ISR_WITHOUT_ERRNO 23
ISR_WITHOUT_ERRNO 24
ISR_WITHOUT_ERRNO 25
ISR_WITHOUT_ERRNO 26
ISR_WITHOUT_ERRNO 27
ISR_WITHOUT_ERRNO 28
ISR_WITHOUT_ERRNO 29
ISR_WITHOUT_ERRNO 30
ISR_WITHOUT_ERRNO 31

;; IRQs
ISR_WITHOUT_ERRNO 32
ISR_WITHOUT_ERRNO 33
ISR_WITHOUT_ERRNO 34
ISR_WITHOUT_ERRNO 35
ISR_WITHOUT_ERRNO 36
ISR_WITHOUT_ERRNO 37
ISR_WITHOUT_ERRNO 38
ISR_WITHOUT_ERRNO 39
ISR_WITHOUT_ERRNO 40
ISR_WITHOUT_ERRNO 41
ISR_WITHOUT_ERRNO 42
ISR_WITHOUT_ERRNO 43
ISR_WITHOUT_ERRNO 44
ISR_WITHOUT_ERRNO 45
ISR_WITHOUT_ERRNO 46
ISR_WITHOUT_ERRNO 47

; Syscall
ISR_WITHOUT_ERRNO 128
ISR_WITHOUT_ERRNO 255

[section .data]
[global isr_redirect_table]
isr_redirect_table:
%assign i 0
%rep 48
	dq isr%+i
%assign i i+1
%endrep

