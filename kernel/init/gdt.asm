;=================================================================================
; Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
;=================================================================================

; 64 bit code
[BITS 64]

; information for linker
section	.text

; share routine with C
global	kernel_gdt_reload

; align routine to full address
align	0x08,	db	0x00
kernel_gdt_reload:
	; preserve original register
	push	rax

	; set CS descriptor
	push	0x08	; offsetof( struct KERNEL_STRUCTURE_GDT, cs_ring0 )
	push	.cs
	retfq

.cs:
	; DS, ES and SS descriptors
	mov	ax,	0x10	; offsetof( struct KERNEL_STRUCTURE_GDT, ds_ring0 )
	mov	ds,	ax
	mov	es,	ax
	mov	ss,	ax

	; FS and GS descriptors (unused by kernel)
	xor	ax,	ax
	mov	fs,	ax
	mov	gs,	ax

	; restore original register
	pop	rax

	; return from the procedure
	ret
