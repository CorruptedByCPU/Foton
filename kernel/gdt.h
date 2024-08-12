/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_GDT
	#define	KERNEL_GDT

	#define	KERNEL_GDT_FIELD_ACCESS_read_or_write		(1 << 1)
	#define	KERNEL_GDT_FIELD_ACCESS_executable		(1 << 3)
	#define	KERNEL_GDT_FIELD_ACCESS_code_or_data		(1 << 4)
	#define	KERNEL_GDT_FIELD_ACCESS_level_1			(1 << 5)
	#define	KERNEL_GDT_FIELD_ACCESS_level_2			(1 << 6)
	#define	KERNEL_GDT_FIELD_ACCESS_level_3			(KERNEL_GDT_FIELD_ACCESS_level_1 | KERNEL_GDT_FIELD_ACCESS_level_2)
	#define	KERNEL_GDT_FIELD_ACCESS_present			(1 << 7)

	#define	KERNEL_GDT_FIELD_FLAGS_long_mode		(1 << 1)

	struct	KERNEL_STRUCTURE_GDT {
		uint64_t	null;
		uint64_t	cs_ring0;
		uint64_t	ds_ring0;
		uint64_t	empty;
		uint64_t	ds_ring3;
		uint64_t	cs_ring3;
		uint64_t	tss;
	} __attribute__( (packed) );

	struct	KERNEL_STRUCTURE_GDT_ENTRY {
		uint16_t	limit_low;
		uint16_t	base_low;
		uint8_t		base_middle;
		uint8_t		access;
		uint8_t		flags_and_limit_high;
		uint8_t		base_high;
	} __attribute__( (packed) );

	struct	KERNEL_STRUCTURE_GDT_HEADER {
		uint16_t				limit;
		struct KERNEL_STRUCTURE_GDT_ENTRY	*base_address;
	} __attribute__( (packed) );

	// external procedure (assembly language)
	extern void kernel_gdt_reload( void );
#endif