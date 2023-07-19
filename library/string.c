/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_STRING
		#include	"string.h"
	#endif

uint64_t lib_string_length( uint8_t *string ) {
	// length of string
	uint64_t length = 0;

	// increment until EMPTY
	while( *(string++) ) length++;

	// return length of string
	return length;
}