/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_INTERFACE
		#include	"./interface.h"
	#endif
	#ifndef	LIB_JSON
		#include	"./json.h"
	#endif

void lib_interface( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// prepare json structure for parsing
	lib_json_squeeze( interface -> properties );

	// convert interface properties to a more accessible format
	lib_interface_convert( interface );
}

void lib_interface_convert( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// properties of new interface format
	uint8_t *properties = (uint8_t *) malloc( lib_string_length( interface -> properties ) );
	uint64_t i = EMPTY;	// properties index
};