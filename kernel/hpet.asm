;===============================================================================
;Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
;===============================================================================

; get pointer from HPET handling function
extern	kernel_hpet_uptime

; 64 bit procedure code
[BITS 64]

; information for linker
section	.text

; share routine
global	kernel_hpet_uptime_entry

; align routine to full address
align	0x08,	db	0x00
kernel_hpet_uptime_entry:
	; keep original registers
	push	rax
	push	rbx
	push	rcx
	push	rdx
	push	rsi
	push	rdi
	push	rbp
	push	r8
	push	r9
	push	r10
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15

	; execute HPET handler
	call	kernel_hpet_uptime

	; restore ogirinal registers
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	rbp
	pop	rdi
	pop	rsi
	pop	rdx
	pop	rcx
	pop	rbx
	pop	rax

	; return from the procedure
	iretq
