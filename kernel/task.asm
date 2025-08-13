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

; debug disassembler
; xchg bx, bx
; int	3

; add al, r8b
; add	[eax + ebx*2 + 0x10], r8b
; add	[rax + rbx*2 + 0x10], r8b
; add ax, r8w
; add eax, r8d
; add rax, r8
; add word [eax + ebx*2 + 0x10], r8w
; add word [rax + rbx*2 + 0x10], r8w
; add dword [eax + ebx*2 + 0x10], r8d
; add dword [rax + rbx*2 + 0x10], r8d
; add qword [eax + ebx*2 + 0x10], r8
; add qword [rax + rbx*2 + 0x10], r8
; add r8b, [eax + ebx*2 + 0x10]
; add r8b, [rax + rbx*2 + 0x10]
; add r8w, word [eax + ebx*2 + 0x10]
; add r8w, word [rax + rbx*2 + 0x10]
; add r8d, dword [eax + ebx*2 + 0x10]
; add r8d, dword [rax + rbx*2 + 0x10]
; add r8, qword [eax + ebx*2 + 0x10]
; add r8, qword [rax + rbx*2 + 0x10]
; add al, 0x7F
; add	ax,	0x1234
; add eax, 0x123456
; add rax, 0x12345678

; push rax
; push rbx
; push rcx
; push rdx
; push rsi
; push rdi
; push rsp
; push rbp
; push r8
; push r9
; push r10
; push r11
; push r12
; push r13
; push r14
; push r15

; pop rax
; pop rbx
; pop rcx
; pop rdx
; pop rsi
; pop rdi
; pop rsp
; pop rbp
; pop r8
; pop r9
; pop r10
; pop r11
; pop r12
; pop r13
; pop r14
; pop r15

; movsxd rax, r8d
; movsxd rax, [r8d + r9d*2 + 0x10]
; movsxd rax, [kernel_task]

; push 0x123456
; push 0x12345678
; push 0x12

; imul eax, ebx, 1
; imul eax, ebx, 0x1234
; imul rax, rbx, 1
; imul rax, rbx, 0x1234
; imul eax, [ebx + 0x10], 0x12
; imul eax, [ebx + 0x10], 0x1234
; imul eax, [rbx + 0x10], 0x12
; imul eax, [rbx + 0x10], 0x1234
; imul rax, [ebx + 0x10], 0x12
; imul rax, [ebx + 0x10], 0x1234
; imul rax, [rbx + 0x10], 0x12
; imul rax, [rbx + 0x10], 0x1234

; rep insb
; rep insw
; rep insd
; rep outsb
; rep outsw
; rep outsd
; insb
; insw
; insd
; outsb
; outsw
; outsd

; jo	kernel_task
; jno	kernel_task
; jb	kernel_task
; jnb	kernel_task
; je	kernel_task
; jne	kernel_task
; jbe	kernel_task
; jnbe	kernel_task
; js	kernel_task
; jns	kernel_task
; jp	kernel_task
; jnp	kernel_task
; jl	kernel_task
; jnl	kernel_task
; jle	kernel_task
; jnle	kernel_task

nop
nop
nop
nop
nop
nop
nop
nop

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
