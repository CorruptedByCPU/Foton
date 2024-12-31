/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_TYPE
		#include	"./type.h"
	#endif

uint8_t lib_type_byte_units[ 5 ] = { ' ', 'K', 'M', 'G', 'T' };

uint8_t lib_type_byte( uint64_t bytes ) {
	// unit type
	uint8_t unit = 0;	// Bytes by default
	while( pow( 1024, unit ) < bytes ) unit++;

	// show higher units?
	if( bytes > 1023 ) return lib_type_byte_units[ unit - 1 ];

	// Bytes
	return lib_type_byte_units[ 0 ];
}