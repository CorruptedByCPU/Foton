;=================================================================================
; Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
;=================================================================================

; get pointer from APIC exception handling function
extern	kernel_task_switch

; 64 bit code
[BITS 64]

; information for linker
section	.text

; share routine
global	kernel_task

; align routine to full address (I am Speed - Lightning McQueen)
align	0x08,	db	0x00
kernel_task:
	; turn off Interrupt Flag
	cli

	; turn off Direction Flag
	cld

; debug disassembler
; xchg	bx,bx
; int	3

; add al, 1
; add al, 0x7F

; add eax, ebx
; add rax, [rcx]
; add r8, r9
; add rbx, [r12+0x20]
; add rax, rbx          ; 64-bit: rax += rbx
; add rbx, [rax]        ; rbx += qword [rax]
; add rcx, [rdx+8]      ; rcx += qword [rdx+8]
; add r8, r9            ; r8 += r9 (requires REX prefix)
; add r10, [r11]        ; r10 += qword [r11]
; add r13, [rsp+16]     ; r13 += qword [rsp+16]
; add rsi, [rdi+rcx*4]  ; rsi += qword [rdi + rcx*4]
; add		al,			cl
; add		bl,			r9b
; add		r8b,		cl
; add eax, ecx
; add	rax, rcx
; add     r10b,	[rax]
; add     r11b,	[r8 + 4]
; add     r12b,	[r9 + rcx]
; add     r12b,	[r9 + rcx + 16]
; add     r12b,	[r9d + ecx + 16]
; add     r12b,	[r9 + rcx*2 + 32]
; add     r12b,	[r9 + rcx*4 + 64]
; add     r12b,	[r9 + rcx*8 + 128]
; add     r13b,	[r10 + 512]
; add     r13b,	[r10 + rdi * 2 + 512]
; add     [rax],					al
; add     [r8 + 4],				bl
; add     [r9 + rcx],				cl
; add     [r9 + rcx + 16],		dl
; add     [r9 + rcx*2 + 32],		r15b
; add     [r9 + rcx*4 + 64],		r14b
; add     [r9 + rcx*8 + 128],		r13b
; add     [r10 + 512],			r12b
; add     [r10 + rdi * 2 + 512],	r11b

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
	mov	rbp,	0xFFFFFFFFFFFFF000
	FXSAVE64	[rbp]

	; execute exception handler
	call	kernel_task_switch

	; restore "floating point" registers
	mov	rbp,	0xFFFFFFFFFFFFF000
	FXRSTOR64	[rbp]

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
