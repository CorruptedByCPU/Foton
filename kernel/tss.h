/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_TSS
	#define	KERNEL_TSS

	struct	KERNEL_TSS_STRUCTURE {
		uint32_t	reserved;
		uint64_t	rsp0;
		uint8_t		unused[ 92 ];
	} __attribute__( (packed) );

	struct	KERNEL_TSS_STRUCTURE_ENTRY {
		uint16_t	limit_low;
		uint16_t	base_low;
		uint8_t		base_middle;
		uint8_t		access;
		uint8_t		flags_and_limit_high;
		uint8_t		base_high;
		uint32_t	base_64bit;
		uint32_t	reserved;
	} __attribute__( (packed) );
#endif