/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_LIBRARY
	#define	KERNEL_LIBRARY

	#define	KERNEL_LIBRARY_base_address		0x0000700000000000

	#define	KERNEL_LIBRARY_FLAG_active		0x01
	#define	KERNEL_LIBRARY_FLAG_reserved		0x80

	#define	KERNEL_LIBRARY_limit			STD_PAGE_byte / sizeof( struct KERNEL_STRUCTURE_LIBRARY )

	struct	KERNEL_STRUCTURE_LIBRARY {
		uint8_t					flags;
		uintptr_t				base;
		uintptr_t				offset;
		uint64_t				limit;
		struct LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL	*elf_section_dynsym;
		uint64_t				elf_section_dynsym_count;
		uint8_t					*elf_section_strtab;
		uint16_t				name_limit;
		uint8_t					name[ LIB_VFS_NAME_limit + 1 ];
	};

	struct KERNEL_STRUCTURE_LIBRARY_INIT {
		uint8_t					level;
		uint8_t					*path;
		uint64_t				limit;
		struct KERNEL_STRUCTURE_VFS_SOCKET	*socket;
		uintptr_t				workbench;
		struct KERNEL_STRUCTURE_LIBRARY		*entry;
		uint64_t				page;
		uintptr_t				base_address;
	};

	// load all libraries, required by this ELF executable
	uint8_t kernel_library( struct LIB_ELF_STRUCTURE *elf );
#endif
