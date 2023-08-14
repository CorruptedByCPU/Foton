/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_IDT
	#define	KERNEL_IDT

	#define	KERNEL_IDT_IRQ_offset			0x20

	#define	KERNEL_IDT_TYPE_gate_interrupt		0x8E00

	struct	KERNEL_IDT_STRUCTURE_ENTRY {
		uint16_t	base_low;
		uint16_t	gdt_descriptor;
		uint16_t	type;
		uint16_t	base_middle;
		uint32_t	base_high;
		uint32_t	reserved;
	} __attribute__( (packed) );

	struct	KERNEL_IDT_STRUCTURE_EXCEPTION {
		uint64_t	cr2;
		uint64_t	r15;
		uint64_t	r14;
		uint64_t	r13;
		uint64_t	r12;
		uint64_t	r11;
		uint64_t	r10;
		uint64_t	r9;
		uint64_t	r8;
		uint64_t	rbp;
		uint64_t	rdi;
		uint64_t	rsi;
		uint64_t	rdx;
		uint64_t	rcx;
		uint64_t	rbx;
		uint64_t	rax;
		uint64_t	id;
		uint64_t	error_code;
		uintptr_t	rip;
		uint64_t	cs;
		uint64_t	eflags;
		uintptr_t	rsp;
		uint64_t	ss;
	} __attribute__( (packed) );

	struct	KERNEL_IDT_STRUCTURE_HEADER {
		uint16_t	limit;
		struct	KERNEL_IDT_STRUCTURE_ENTRY	*base_address;
	} __attribute__( (packed) );

	struct	KERNEL_IDT_STRUCTURE_RETURN {
		uintptr_t	rip;
		uint64_t	cs;
		uint64_t	eflags;
		uintptr_t	rsp;
		uint64_t	ss;
	} __attribute__( (packed) );

	// connect interrupt occurrence with corresponding procedure
	void kernel_idt_mount( uint8_t id, uint16_t type, uintptr_t address );

	// default interrupt procedure for any device/service
	__attribute__ (( preserve_all ))
	void kernel_idt_interrupt_default( struct KERNEL_IDT_STRUCTURE_RETURN *interrupt );

	// external routine (assembly language)
	extern void kernel_irq( void );

	// external routines (assembly language)
	extern void kernel_idt_exception_divide_by_zero( void );
	extern void kernel_idt_exception_debug( void );
	extern void kernel_idt_exception_breakpoint( void );
	extern void kernel_idt_exception_overflow( void );
	extern void kernel_idt_exception_boud_range_exceeded( void );
	extern void kernel_idt_exception_invalid_opcode( void );
	extern void kernel_idt_exception_device_not_available( void );
	extern void kernel_idt_exception_double_fault( void );
	extern void kernel_idt_exception_coprocessor_segment_overrun( void );
	extern void kernel_idt_exception_invalid_tss( void );
	extern void kernel_idt_exception_segment_not_present( void );
	extern void kernel_idt_exception_stack_segment_fault( void );
	extern void kernel_idt_exception_general_protection_fault( void );
	extern void kernel_idt_exception_page_fault( void );
	extern void kernel_idt_exception_x87_floating_point( void );
	extern void kernel_idt_exception_alignment_check( void );
	extern void kernel_idt_exception_machine_check( void );
	extern void kernel_idt_exception_simd_floating_point( void );
	extern void kernel_idt_exception_virtualization( void );
	extern void kernel_idt_exception_control_protection( void );
	extern void kernel_idt_exception_hypervisor_injection( void );
	extern void kernel_idt_exception_vmm_communication( void );
	extern void kernel_idt_exception_security( void );
	extern void kernel_idt_exception_triple_fault( void );

	// external routine (assembly language)
	extern void kernel_idt_spurious_interrupt( void );
#endif
