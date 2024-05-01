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
extern	kernel_syscall_file_touch
extern	kernel_syscall_task
extern	kernel_syscall_kill
extern	kernel_syscall_network_interface
extern	kernel_syscall_network_open
extern	kernel_syscall_network_send

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
	dq	kernel_syscall_exit			; 0x00
	dq	kernel_syscall_framebuffer		; 0x01
	dq	kernel_syscall_memory_alloc		; 0x02
	dq	kernel_syscall_memory_release		; 0x03
	dq	kernel_syscall_uptime			; 0x04
	dq	kernel_syscall_log			; 0x05
	dq	kernel_syscall_thread			; 0x06
	dq	kernel_syscall_pid			; 0x07
	dq	kernel_syscall_exec			; 0x08
	dq	kernel_syscall_pid_check		; 0x09
	dq	kernel_syscall_ipc_send			; 0x0A
	dq	kernel_syscall_ipc_receive		; 0x0B
	dq	kernel_syscall_memory_share		; 0x0C
	dq	kernel_syscall_mouse			; 0x0D
	dq	kernel_syscall_framebuffer_change	; 0x0E
	dq	kernel_syscall_ipc_receive_by_pid	; 0x0F
	dq	kernel_syscall_stream_out		; 0x10
	dq	kernel_syscall_stream_in		; 0x11
	dq	kernel_syscall_keyboard			; 0x12
	dq	kernel_syscall_stream_set		; 0x13
	dq	kernel_syscall_stream_get		; 0x14
	dq	kernel_syscall_memory			; 0x15
	dq	kernel_syscall_sleep			; 0x16
	dq	kernel_syscall_file_open		; 0x17
	dq	kernel_syscall_file_close		; 0x18
	dq	kernel_syscall_cd			; 0x19
	dq	kernel_syscall_ipc_receive_by_type	; 0x1A
	dq	kernel_syscall_microtime		; 0x1B
	dq	kernel_syscall_time			; 0x1C
	dq	kernel_syscall_file_read		; 0x1D
	dq	kernel_syscall_file			; 0x1E
	dq	kernel_syscall_file_write		; 0x1F
	dq	kernel_syscall_file_touch		; 0x20
	dq	kernel_syscall_task			; 0x21
	dq	kernel_syscall_kill			; 0x22
	dq	kernel_syscall_network_interface	; 0x23
	dq	kernel_syscall_network_open		; 0x24
	dq	kernel_syscall_network_send		; 0x25
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