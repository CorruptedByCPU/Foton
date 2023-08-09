/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_STD
		#include	"./std.h"
	#endif

	//----------------------------------------------------------------------
	// std routines, procedures
	//----------------------------------------------------------------------
	#include	"./std/syscall.c"

	//------------------------------------------------------------------------------
	// substitute of libc
	//------------------------------------------------------------------------------

void *malloc( size_t byte ) {
	// assign place for area of definied size
	uint64_t *target = (uint64_t *) std_memory_alloc( byte + 0x10 );

	// set metadata of area
	*target = MACRO_PAGE_ALIGN_UP( byte + 0x10 ) >> STD_SHIFT_PAGE;

	// return allocated area pointer
	return target + 0x02;
}

void free( void *source ) {
	// move pointer to metadata area
	uint64_t *byte = source - 0x10;

	// release assigned area
	std_memory_release( (uintptr_t) byte, *byte );
}