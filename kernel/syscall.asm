;=================================================================================
; Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
;=================================================================================

;------------------------------------------------------------------------------
; get pointers from kernel environment
;------------------------------------------------------------------------------
extern	kernel_syscall_exit
extern	kernel_syscall_framebuffer
extern	kernel_syscall_memory_alloc
extern	kernel_syscall_memory_release
extern	kernel_syscall_uptime
extern	kernel_syscall_log
extern	kernel_syscall_thread
extern	kernel_syscall_pid
extern	kernel_syscall_exec
extern	kernel_syscall_pid_check
extern	kernel_syscall_ipc_send
extern	kernel_syscall_ipc_receive
extern	kernel_syscall_memory_share
extern	kernel_syscall_mouse
extern	kernel_syscall_framebuffer_change
extern	kernel_syscall_ipc_receive_by_pid
extern	kernel_syscall_stream_out
extern	kernel_syscall_stream_in
extern	kernel_syscall_keyboard
extern	kernel_syscall_stream_set
extern	kernel_syscall_stream_get
extern	kernel_syscall_memory
extern	kernel_syscall_sleep
extern	kernel_syscall_file_open
extern	kernel_syscall_file_close
extern	kernel_syscall_cd
extern	kernel_syscall_ipc_receive_by_type
extern	kernel_syscall_microtime
extern	kernel_syscall_time
extern	kernel_syscall_file_read
extern	kernel_syscall_file
extern	kernel_syscall_file_write

;------------------------------------------------------------------------------
; share routines and list
;------------------------------------------------------------------------------
global	kernel_syscall
global	kernel_syscall_list

; information for linker
section	.rodata

; align routine to full address
align	0x08,	db	0x00
kernel_syscall_list:
	dq	kernel_syscall_exit
	dq	kernel_syscall_framebuffer
	dq	kernel_syscall_memory_alloc
	dq	kernel_syscall_memory_release
	dq	kernel_syscall_uptime
	dq	kernel_syscall_log
	dq	kernel_syscall_thread
	dq	kernel_syscall_pid
	dq	kernel_syscall_exec
	dq	kernel_syscall_pid_check
	dq	kernel_syscall_ipc_send
	dq	kernel_syscall_ipc_receive
	dq	kernel_syscall_memory_share
	dq	kernel_syscall_mouse
	dq	kernel_syscall_framebuffer_change
	dq	kernel_syscall_ipc_receive_by_pid
	dq	kernel_syscall_stream_out
	dq	kernel_syscall_stream_in
	dq	kernel_syscall_keyboard
	dq	kernel_syscall_stream_set
	dq	kernel_syscall_stream_get
	dq	kernel_syscall_memory
	dq	kernel_syscall_sleep
	dq	kernel_syscall_file_open
	dq	kernel_syscall_file_close
	dq	kernel_syscall_cd
	dq	kernel_syscall_ipc_receive_by_type
	dq	kernel_syscall_microtime
	dq	kernel_syscall_time
	dq	kernel_syscall_file_read
	dq	kernel_syscall_file
	dq	kernel_syscall_file_write
kernel_syscall_list_end:

; 64 bit procedure code
[BITS 64]

; information for linker
section	.text

; align routine to full address
align	0x08,	db	0x00
kernel_syscall:
	; keep RIP and EFLAGS registers of process
	xchg	qword [rsp + 0x08],	rcx
	xchg	qword [rsp],	r11

	; feature available?
	cmp	rax,	(kernel_syscall_list_end - kernel_syscall_list) / 0x08
	jb	.available	; yes

	; no kernel feature selected
	mov	rax,	-1

	; go back to process
	jmp	.return

.available:
	; preserve original registers
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

	; execute kernel function according to parameter in RAX
	call	qword [kernel_syscall_list + rax * 0x08]

	; restore original registers
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

.return:
	; restore the RIP and EFLAGS registers of the process
	xchg	qword [rsp],	r11
	xchg	qword [rsp + 0x08],	rcx

	; return to process code
	o64	sysret