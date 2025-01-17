;=================================================================================
; Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
;=================================================================================

; get pointer from driver handling function
extern	module_virtio_net

; 64 bit procedure code
[BITS 64]

; information for linker
section	.text

; share routine
global	module_virtio_net_entry

; align routine to full address
align	0x08,	db	0x00
module_virtio_net_entry:
	; turn off Direction Flag
	cld

	; preserve original registers
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

	; preserve "floating point" registers
	mov	rax,	-0x1000
	FXSAVE64	[rax]

	; execute driver handler
	call	module_virtio_net

	; restore "floating point" registers
	mov	rax,	-0x1000
	FXRSTOR64	[rax]

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

	; return from routine
	iretq