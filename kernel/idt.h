/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_IDT
	#define	KERNEL_IDT

	// devices IRQ line starts at
	#define	KERNEL_IDT_IRQ_offset			0x20

	#define	KERNEL_IDT_TYPE_gate_interrupt		0x8E00
	#define	KERNEL_IDT_TYPE_irq			0x8F00
	#define	KERNEL_IDT_TYPE_isr			0xEF00

	#define	KERNEL_IDT_EFLAGS_CF		0x00000001
	#define	KERNEL_IDT_EFLAGS_PF		0x00000004
	#define	KERNEL_IDT_EFLAGS_AF		0x00000010
	#define	KERNEL_IDT_EFLAGS_ZF		0x00000040
	#define	KERNEL_IDT_EFLAGS_SF		0x00000080
	#define	KERNEL_IDT_EFLAGS_TF		0x00000100
	#define	KERNEL_IDT_EFLAGS_IF		0x00000200
	#define	KERNEL_IDT_EFLAGS_DF		0x00000400
	#define	KERNEL_IDT_EFLAGS_OF		0x00000800
	#define	KERNEL_IDT_EFLAGS_IOPL		0x00003000
	#define	KERNEL_IDT_EFLAGS_NT		0x00004000
	#define	KERNEL_IDT_EFLAGS_RF		0x00010000
	#define	KERNEL_IDT_EFLAGS_VM		0x00020000
	#define	KERNEL_IDT_EFLAGS_AC		0x00040000
	#define	KERNEL_IDT_EFLAGS_VIF		0x00080000
	#define	KERNEL_IDT_EFLAGS_VIP		0x00100000
	#define	KERNEL_IDT_EFLAGS_ID		0x00200000

	struct	KERNEL_STRUCTURE_IDT_ENTRY {
		uint16_t				base_low;
		uint16_t				gdt_descriptor;
		uint16_t				type;
		uint16_t				base_middle;
		uint32_t				base_high;
		uint32_t				reserved;
	} __attribute__( (packed) );

	struct	KERNEL_STRUCTURE_IDT_EXCEPTION {
		uint64_t				cr2;
		uint64_t				r15;
		uint64_t				r14;
		uint64_t				r13;
		uint64_t				r12;
		uint64_t				r11;
		uint64_t				r10;
		uint64_t				r9;
		uint64_t				r8;
		uint64_t				rbp;
		uint64_t				rdi;
		uint64_t				rsi;
		uint64_t				rdx;
		uint64_t				rcx;
		uint64_t				rbx;
		uint64_t				rax;
		uint64_t				id;
		uint64_t				error_code;
		uintptr_t				rip;
		uint64_t				cs;
		uint64_t				eflags;
		uintptr_t				rsp;
		uint64_t				ss;
	} __attribute__( (packed) );

	struct	KERNEL_STRUCTURE_IDT_HEADER {
		uint16_t				limit;
		struct	KERNEL_STRUCTURE_IDT_ENTRY	*base_address;
	} __attribute__( (packed) );

	struct	KERNEL_STRUCTURE_IDT_RETURN {
		uintptr_t				rip;
		uint64_t				cs;
		uint64_t				eflags;
		uintptr_t				rsp;
		uint64_t				ss;
	} __attribute__( (packed) );

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
	extern void kernel_idt_interrupt( void );

	// external routine (assembly language)
	extern void kernel_idt_spurious_interrupt( void );

	// connect IRQ [type] to function
	void kernel_idt_attach( uint8_t irq, uint16_t type, uintptr_t address );
#endif
