;=================================================================================
; Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
;=================================================================================

; get pointer from driver handling function
extern	kernel_rtc

; 64 bit procedure code
[BITS 64]

; information for linker
section	.text

; share routine
global	driver_rtc_entry

; align routine to full address
align	0x08,	db	0x00
driver_rtc_entry:
	; turn off Direction Flag
	cld

	; preserve original register
	push	r11

	; execute driver handler
	call	kernel_rtc

	; restore original register
	pop	r11

	; return from the procedure
	iretq