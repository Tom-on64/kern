bits 32

;; Prototype:
;;	void switch_context(struct task* old, struct task* new);
;; Description:
;; 	Pushes register state onto the old tasks kstack and pops the new state.
global task_switch
task_switch:
	mov eax, [esp + 4]	; eax = struct task* old
	mov edx, [esp + 8]	; ebx = struct task* new

	;; Push registers that weren't pushed by the CDECL call
	push ebp
	push ebx
	push esi
	push edi

	;; Swap the stack pointers
	mov [eax + 4], esp	; old->kstack_pointer = ESP
	mov esp, [edx + 4]	; ESP = new->kstack_pointer

	;; Change the pagedir
	mov eax, [edx + 12]	; EAX = new->pagedir
	sub eax, 0xC0000000	; Convert to physical address
	mov cr3, eax

	;; Pop register that weren't pushed by the CDECL call
	pop edi
	pop esi
	pop ebx
	pop ebp

	;; This return will pop task_switch_ctx.eip
	ret

