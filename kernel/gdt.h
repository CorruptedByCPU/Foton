/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_GDT
	#define	KERNEL_GDT

	struct	KERNEL_GDT_STRUCTURE {
		uint64_t	null;
		uint64_t	cs_ring0;
		uint64_t	ds_ring0;
		uint64_t	empty;
		uint64_t	ds_ring3;
		uint64_t	cs_ring3;
		uint64_t	tss;
	} __attribute__( (packed) );

	struct	KERNEL_GDT_STRUCTURE_ENTRY {
		uint16_t	limit_low;
		uint16_t	base_low;
		uint8_t		base_middle;
		uint8_t		access;
		uint8_t		flags_and_limit_high;
		uint8_t		base_high;
	} __attribute__( (packed) );

	struct	KERNEL_GDT_STRUCTURE_HEADER {
		uint16_t				limit;
		struct KERNEL_GDT_STRUCTURE_ENTRY	*base_address;
	} __attribute__( (packed) );

	// external procedure (assembly language)
	extern void kernel_gdt_reload( void );
#endif