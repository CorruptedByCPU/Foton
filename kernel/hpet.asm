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
	push	r11

	; execute HPET handler
	call	kernel_hpet_uptime

	; restore ogirinal registers
	pop	r11

	; return from the procedure
	iretq
