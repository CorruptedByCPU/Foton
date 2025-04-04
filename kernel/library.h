/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_LIBRARY
	#define	KERNEL_LIBRARY

	#define	KERNEL_LIBRARY_base_address		0x0000700000000000

	#define	KERNEL_LIBRARY_FLAG_active		0b00000001
	#define	KERNEL_LIBRARY_FLAG_reserved		0b10000000

	#define	KERNEL_LIBRARY_limit			(MACRO_PAGE_ALIGN_UP( STD_PAGE_byte / sizeof( struct KERNEL_STRUCTURE_LIBRARY ) ) >> STD_SHIFT_PAGE)

	struct	KERNEL_STRUCTURE_LIBRARY {
		uint8_t					flags;
		uintptr_t				offset;
		uint64_t				limit;
		uint16_t				name_length;
		uint8_t					name[ LIB_VFS_NAME_limit ];
	};
#endif
