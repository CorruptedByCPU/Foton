/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_LIBRARY
	#define	KERNEL_LIBRARY

	#define	KERNEL_LIBRARY_base_address	0x0000700000000000

	#define	KERNEL_LIBRARY_limit		16

	struct	KERNEL_LIBRARY_STRUCTURE {
		uintptr_t	pointer;
		uint64_t	size_page;
		uint8_t		length;
		uint8_t		name[ 255 ];
	};
#endif
