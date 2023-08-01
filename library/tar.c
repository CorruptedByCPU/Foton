/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_TAR
		#include	"tar.h"
	#endif

uint8_t ustarX[ 6 ] = "ustar\x0";

uint8_t lib_tar_check( uintptr_t address ) {
	// properties of file
	struct LIB_TAR_STRUCTURE_header *header = (struct LIB_TAR_STRUCTURE_header *) address;

	// "ustar" marker?
	if( lib_string_compare( ustarX, header -> magic, 6 ) ) return TRUE;	// yes

	// no
	return FALSE;
}