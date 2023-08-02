;=================================================================================
; Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
;=================================================================================

;------------------------------------------------------------------------------
; get pointers from kernel environment
;------------------------------------------------------------------------------
; extern	kernel_syscall_exit

;------------------------------------------------------------------------------
; share routines and list
;------------------------------------------------------------------------------
global	kernel_syscall
global	kernel_syscall_list

; information for linker
section	.rodata

; align routine to full address
align	0x08,	db	0x00
kernel_syscall_list:
	; dq	kernel_syscall_exit
kernel_syscall_list_end:

; 64 bit procedure code
[BITS 64]

; information for linker
section	.text

; align routine to full address
align	0x08,	db	0x00
kernel_syscall:
	; keep RIP and EFLAGS registers of process
	xchg	qword [rsp + 0x08],	rcx
	xchg	qword [rsp],	r11

	; feature available?
	cmp	rax,	(kernel_syscall_list_end - kernel_syscall_list) / 0x08
	jbe	.available	; yes

	; no kernel feature selected
	mov	rax,	-1

	; go back to process
	jmp	.return

.available:
	; execute kernel function according to parameter in RAX
	call	qword [kernel_syscall_list + rax * 0x08]

.return:
	; restore the RIP and EFLAGS registers of the process
	xchg	qword [rsp],	r11
	xchg	qword [rsp + 0x08],	rcx

	; return to process code
	o64	sysret