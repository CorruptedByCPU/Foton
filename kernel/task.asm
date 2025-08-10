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

; ; debug disassembler
; xchg	bx,bx
; int	3

; .test:
; imul eax, ebx, 1
; imul eax, ebx, 0x12345678
; imul rax, rbx, 1
; imul rax, rbx, 0x12345678
; imul eax, [ebx], 1
; imul eax, [rbx], 1
; imul rax, [ebx], 1
; imul rax, [rbx], 1
; imul eax, [ebx], 0x12345678
; imul eax, [rbx], 0x12345678
; imul rax, [ebx], 0x12345678
; imul rax, [rbx], 0x12345678
; nop
; push 0x12
; push 0x1234
; push 0x12345678
; nop
; movsxd rax, ebx
; movsxd rax, [ebx]
; movsxd rax, [rbx]
; movsxd rax, [kernel_task.test]
; nop
; add al, cl
; add ax,	cx
; add eax, ecx
; add rax, rcx
; add al, byte [ecx]
; add al, byte [ecx + 16]
; add al, byte [ecx + r15d]
; add al, byte [ecx + r15d + 16]
; add al, byte [ecx + r15d*2 + 16]
; add al, byte [r13]
; add al, byte [r13 + 16]
; add al, byte [r13 + rdx]
; add al, byte [r13 + rdx + 16]
; add al, byte [r13 + rdx*2 + 16]
; add [eax], ecx
; add [eax], rcx
; add [rax], ecx
; add [rax], rcx


	; turn off Interrupt Flag
	cli

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
