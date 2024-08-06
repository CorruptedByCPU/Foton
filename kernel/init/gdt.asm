;=================================================================================
; Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
;=================================================================================

; 64 bit procedure code
[BITS 64]

; information for linker
section	.text

; share initialization routine
global	kernel_gdt_reload

; align routine to full address
align	0x08,	db	0x00
kernel_gdt_reload:
	; preserve original register
	push	rax

	; CS descriptor
	push	0x08
	push	.cs
	retfq

.cs:
	; DS, ES and SS descriptors
	mov	ax,	0x10
	mov	ds,	ax
	mov	es,	ax
	mov	ss,	ax

	; FS and GS descriptors
	xor	ax,	ax
	mov	fs,	ax
	mov	gs,	ax

	; restore original register
	pop	rax

	; return from the procedure
	ret
