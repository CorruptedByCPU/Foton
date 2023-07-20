/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_ELF
		#include	"elf.h"
	#endif

uint8_t lib_elf_identify( uintptr_t address ) {
	// file properties
	struct LIB_ELF_STRUCTURE *header = (struct LIB_ELF_STRUCTURE *) address;

	// magic number exist at beginning of file?
	if( header -> magic_number != LIB_ELF_HEADER_magic_number ) return FALSE;	// no

	// yes
	return TRUE;
}