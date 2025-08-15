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
; add [eax + ebx*2 + 0x10], r8b
; add [rax + rbx*2 + 0x10], r8b
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
; add ax, 0x1234
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
; movsxd rax, r8d	; WHY THE HELL ONLY YOU! FROM ALL OPCODES
; movsxd rax, [r8d + r9d*2 + 0x10]
; movsxd rax, [r8 + r9*2 - 0x10]
; movsxd rax, [kernel_task]
; push 0x12
; push 0x123456
; push 0x12345678
; imul eax, ebx, 1
; imul eax, ebx, 0x1234
; imul rax, rbx, 1
; imul rax, rbx, 0x1234
; imul eax, [ebx + ecx*2 + 0x10], 0x12
; imul eax, [ebx + ecx*2 + 0x10], 0x1234
; imul eax, [rbx + rcx*2 + 0x10], 0x12
; imul eax, [rbx + rcx*2 + 0x10], 0x1234
; imul rax, [ebx + ecx*2 + 0x10], 0x12
; imul rax, [ebx + ecx*2 + 0x10], 0x1234
; imul rax, [rbx + rcx*2 + 0x10], 0x12
; imul rax, [rbx + rcx*2 + 0x10], 0x1234
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
; db 0x70, 0xF6
; db 0x71, 0xF4
; db 0x72, 0xF2
; db 0x73, 0xF0
; db 0x74, 0xEE
; db 0x75, 0xEC
; db 0x76, 0xEA
; db 0x77, 0xE8
; db 0x78, 0xE6
; db 0x79, 0xE4
; db 0x7A, 0xE2
; db 0x7B, 0xE0
; db 0x7C, 0xDE
; db 0x7D, 0xDC
; db 0x7E, 0xDA
; db 0x7F, 0xD8
; db 0x80, 0xC0, 0x12 ; add al, 0x12
; add byte [eax + ebx*2 + 0x10], 0x12
; add byte [rax + rbx*2 + 0x10], 0x12
; db 0x66, 0x81, 0xC0, 0x34, 0x12 ; add ax, 0x1234
; db 0x81, 0xC0, 0x78, 0x56, 0x34, 0x12 ; add eax, 0x12345678
; db 0x48, 0x81, 0xC0, 0x78, 0x56, 0x34, 0x12 ; add rax, 0x12345678
; add word [eax + ebx*2 + 0x10], 0x1234
; add word [rax + rbx*2 + 0x10], 0x1234
; db 0x81, 0x05, 0x08, 0x00, 0x00, 0x00, 0x78, 0x56, 0x34, 0x12 ; add dword [rip + 0x08], 0x12345678
; db 0x81, 0x05, 0xFB, 0xFF, 0xFF, 0xFF, 0x78, 0x56, 0x34, 0x12 ; add dword [rip - 0x05], 0x12345678
; db 0x83, 0xC0, 0x12	; add eax, 0x12
; add rax, 0x12
; db 0x67, 0x83, 0x00, 0xFF ; add dword [eax], 0xFF
; db 0x83, 0x00, 0xFF ; add dword [rax], 0xFF
; test al, bl
; test byte [eax + ecx*2 + 0x10], bl
; test byte [rax + rcx*2 + 0x10], bl
; test ax, r8w
; test eax, r8d
; test rax, r8
; test word [eax + ebx*2 + 0x10], r8w
; test word [rax + rbx*2 + 0x10], r8w
; test dword [eax + ebx*2 + 0x10], r8d
; test dword [rax + rbx*2 + 0x10], r8d
; test qword [eax + ebx*2 + 0x10], r8
; test qword [rax + rbx*2 + 0x10], r8
; xchg word [eax + ebx*2 + 0x10], r8w
; xchg word [rax + rbx*2 + 0x10], r8w
; xchg dword [eax + ebx*2 + 0x10], r8d
; xchg dword [rax + rbx*2 + 0x10], r8d
; xchg qword [eax + ebx*2 + 0x10], r8
; xchg qword [rax + rbx*2 + 0x10], r8
; mov al, r8b
; mov byte [eax + ebx*2 + 0x10], r8b
; mov byte [rax + rbx*2 + 0x10], r8b
; mov ax, r8w
; mov eax, r8d
; mov rax, r8
; mov word [eax + ebx*2 + 0x10], r8w
; mov word [rax + rbx*2 + 0x10], r8w
; mov dword [eax + ebx*2 + 0x10], r8d
; mov dword [rax + rbx*2 + 0x10], r8d
; mov qword [eax + ebx*2 + 0x10], r8
; mov qword [rax + rbx*2 + 0x10], r8
; mov r8b, al
; mov byte [eax + ebx*2 + 0x10], r8b
; mov byte [rax + rbx*2 + 0x10], r8b
; mov r8w, ax
; mov r8d, eax
; mov r8, rax
; mov r8w, word [eax + ebx*2 + 0x10]
; mov r8w, word [rax + rbx*2 + 0x10]
; mov r8d, dword [eax + ebx*2 + 0x10]
; mov r8d, dword [rax + rbx*2 + 0x10]
; mov r8, qword [eax + ebx*2 + 0x10]
; mov r8, qword [rax + rbx*2 + 0x10]
; lea r8d, [eax + ebx*2 + 0x10]
; lea r8d, [rax + rbx*2 + 0x10]
; lea r8, [eax + ebx*2 + 0x10]
; lea r8, [rax + rbx*2 + 0x10]
; pop word [r8d + 0x10]
; pop word [r8 + 0x10]
; pop word [0x12345678]
; pop qword [r8d + 0x08]
; pop qword [r8 + 0x10]
; pop qword [0x12345678]
; xchg r8w, ax
; xchg r8d, eax
; xchg r8, rax
; cbw
; cwde
; cdqe
; cwd
; cdq
; cqo
; pushfq
; popfq
; sahf
; lahf

; displacement test
; mov eax, [0x12345678]
; mov eax, [ebp + 8]
; mov eax, [ebp - 4]
; mov eax, [esi + 0x1000]
; mov eax, [edi - 0x2000]
; mov eax, [ebx + esi*4 + 16]
; mov eax, [r13 + rdx*8 - 32]
; mov eax, [rax + rcx*2 + 0x1234]
; mov eax, [esi*4 + 0x10]
; mov eax, [rdx*8 - 0x20]
; mov eax, [ecx*2 + 0x100]
; mov eax, [ebx + 16]
; mov eax, [r12 + 0x200]

; db 0xA0, 0x05, 0x0B, 0x00, 0x00, 0x00 ; mov al, [rip + 0x0B]
; db 0x48, 0xA1, 0x78, 0x56, 0x34, 0x12 ; mov rax, [0x12345678]
; db 0xA2, 0x05, 0x0B, 0x00, 0x00, 0x00 ; mov al, [rip + 0x0B]
; db 0x48, 0xA3, 0x78, 0x56, 0x34, 0x12 ; mov rax, [0x12345678]

; nop
; nop
; nop
; nop
; nop
; nop
; nop
; nop

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
