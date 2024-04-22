;=================================================================================
; Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
;=================================================================================

; get pointer from driver handling function
extern	driver_e1000

; 64 bit procedure code
[BITS 64]

; information for linker
section	.text

; share routine
global	driver_e1000_entry

; align routine to full address
align	0x08,	db	0x00
driver_e1000_entry:
	; turn off Direction Flag
	cld

	; execute driver handler
	call	driver_e1000

	; return from the procedure
	iretq
