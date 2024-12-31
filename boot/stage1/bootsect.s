; KBOOT - Kern Bootloader
; Most of the code is taken from Limine (https://github.com/limine-bootloader/limine)
; I tried to add more documentation than the Limine code had, but it's not everything
; Any of my code is pretty bad, so i recommend using an existing bootloader

[org 0x7c00]
[bits 16]

; BPB Requires these
jmp short .bpb_end
nop

; FAT32 Bios Parameter Block
.bpb:
	.bpb_oem_id:		db "KERN    "
	.bpb_sector_size:	dw 0
	.bpb_sects_per_clust:	db 0
	.bpb_reserved_sects:	dw 0
	.bpb_fat_count:		db 0
	.bpb_root_dir_entries:	dw 0
	.bpb_sector_count:	dw 0
	.bpb_media_type:	db 0
	.bpb_sects_per_fat:	dw 0
	.bpb_sects_per_track:	dw 18
	.bpb_heads_count:	dw 2
	.bpb_hidden_sects:	dd 0
	.bpb_sect_count_big:	dd 0
; Extended BPB
	.bpb_drive_num:		db 0
	.bpb_reserved:		db 0
	.bpb_signature:		db 0
	.bpb_volume_id:		dd 0
	.bpb_volume_label:	dd "KERN       "
	.bpb_fs_type:		times 8 db 0
.bpb_end:
	cli
	cld
	jmp 0x0000:.init_codeseg    ; Long jump to set CS = 0

	; Zero out segment regs (Because segmentation is shit)
.init_codeseg: 
	xor si, si      ; We cannot change segment regs directly  
	mov ds, si      ; Data Segment
	mov es, si      ; Extra Segment
	mov ss, si      ; Stack Segment
	mov sp, 0x7c00  ; Set Stack Pointer
	sti

	; We probably won't be booting from a floppy, so
	; ignore if boot drive is < 0x80 and assume an error
	cmp dl, 0x80
	jl err.0
	; Also anything above 0x8f is "dubious" so we also 
	; just assume an error
	cmp dl, 0x8f
	jg err.1

	; Check that int 13h extensions are supported
.int13chk:
	mov ah, 0x41
	mov bx, 0x55aa  ; Expects bx to be this magic value
	int 0x13        ; Will se CF if unsupported
	jc err.2
	cmp bx, 0xaa55  ; Also sets bx to this magic
	jne err.3

	; Load the stage 2 bootloader code to 0x70000
.load_stage2:
	mov eax, 1	; LBA
	mov cx, 20	; Read 20 sectors (we have space for 2048)

	; Destination - 0x70000
	push 0x7000
	pop es
	xor bx, bx

	call read_sectors
	jc err.4	; Failed to read stage 2

.enable_protected:
	; Load GDT Pointer
	lgdt [gdtp]

	; Set PE (Protected mode Enable) flag
	cli
	mov eax, cr0
	or eax, 1   ; Bit 1 - PE
	mov cr0, eax

	; Far jump to protected mode
	jmp code_seg:vector

%include "stage1/disk.s"   ; Disk I/O helper functions
%include "stage1/gdt.s"    ; Global Descriptor Table

; Handles an error
err:
	; Error codes
	.4: inc si  ; Failed to read stage2 from disk
	.3: inc si  ; Int 13h extensions unsupported (BX wasn't magic)
	.2: inc si  ; Int 13h extensions unsupported (CF was set)
	.1: inc si  ; Invalid boot drive number (> 0x8f)
	.0:         ; Invalid boot drive number (< 0x80)
	; Common error handler
	add si, '0' | (0x4f << 8)   ; Store error code number char in SI, white on red 
	push 0xb800 ; VGA Text mode video memory
	pop es      ; ES = 0xb800
	mov word [es:0], si ; Write character

	; Hang
	sti
.h: hlt
	jmp .h

	[bits 32]
	; Protected mode entry vector
vector: 
	mov eax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	and edx, 0xff

	; Call stage 2 bootloader
	call 0x70000

; Padding and boot signature
times 510-($-$$) db 0
dw 0xaa55 
