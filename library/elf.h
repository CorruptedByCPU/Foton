/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_ELF
	#define	LIB_ELF

	#define	LIB_ELF_HEADER_magic_number	0x464C457F	// 0x7F, 'E, 'L', 'F'

	#define	LIB_ELF_FLAG_execute		0b00000001	// segment is executable
	#define	LIB_ELF_FLAG_write		0b00000010	// segment is writable

	#define	LIB_ELF_TYPE_unknown		0x00
	#define	LIB_ELF_TYPE_relocatable	0x01
	#define	LIB_ELF_TYPE_executable		0x02
	#define	LIB_ELF_TYPE_shared_object	0x03

	#define	LIB_ELF_HEADER_TYPE_load	1

	#define	LIB_ELF_SECTION_TYPE_null	0x00
	#define	LIB_ELF_SECTION_TYPE_progbits	0x01
	#define	LIB_ELF_SECTION_TYPE_rela	0x04
	#define	LIB_ELF_SECTION_TYPE_strtab	0x03
	#define	LIB_ELF_SECTION_TYPE_dynamic	0x06
	#define	LIB_ELF_SECTION_TYPE_dynsym	0x0B

	#define	LIB_ELF_SECTION_DYNAMIC_TYPE_needed	1

	struct	LIB_ELF_STRUCTURE {
		uint32_t	magic_number;
		uint8_t		class;
		uint8_t		memory_layout;
		uint8_t		header_version;
		uint8_t		abi;
		uint8_t		abi_version;
		uint8_t		padding[ 7 ];
		uint16_t	type;
		uint16_t	instruction_set;
		uint32_t	elf_version;

		uint64_t	entry_ptr;
		uint64_t	headers_offset;
		uint64_t	sections_offset;

		uint32_t	flags;
		uint16_t	header_size;
		uint16_t	header_table_size;
		uint16_t	h_entry_count;
		uint16_t	section_table_size;
		uint16_t	s_entry_count;
		uint16_t	correlation;
	} __attribute__( (packed) );

	struct	LIB_ELF_STRUCTURE_HEADER {
		uint32_t	type;
		uint32_t	flags;
		uint64_t	segment_offset;
		uint64_t	virtual_address;
		uint64_t	physical_address;
		uint64_t	segment_size;
		uint64_t	memory_size;
		uint64_t	alignment;
	} __attribute__( (packed) );

	struct	LIB_ELF_STRUCTURE_SECTION {
		uint32_t	name;
		uint32_t	type;
		uint64_t	flags;
		uint64_t	virtual_address;
		uint64_t	file_offset;
		uint64_t	size_byte;
		uint32_t	link;
		uint32_t	info;
		uint64_t	alignment;
		uint64_t	entry_count;
	} __attribute__( (packed) );

	struct	LIB_ELF_STRUCTURE_DYNAMIC_ENTRY {
		uint64_t	type;
		uint64_t	name_offset;
	} __attribute__( (packed) );

	struct	LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL {
		uint32_t	name_offset;
		uint32_t	flags;
		uint64_t	address;
		uint64_t	size_byte;
	} __attribute__( (packed) );

	struct	LIB_ELF_STRUCTURE_DYNAMIC_RELOCATION {
		uint64_t	offset;
		uint32_t	flags;
		uint32_t	index;
		uint64_t	symbol_value;
	} __attribute__( (packed) );
#endif