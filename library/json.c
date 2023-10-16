/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_JSON
		#include	"./json.h"
	#endif

uint64_t lib_json_squeeze( uint8_t *json ) {
	// pointer to formatted json
	uint8_t *formatted = json;
	uint64_t i = EMPTY;	// formatted index

	// calculate new length
	do {
		// start of quote?
		if( *json == STD_ASCII_QUOTATION ) {
			// keep previous value
			uint8_t previous = *(json - 1);

			// count towards
			*(formatted++) = *(json++); i++;

			// look for end of quote
			while( *json != STD_ASCII_QUOTATION || previous == STD_ASCII_BACKSLASH ) {
				// update previous value
				previous = *json;
				
				// count towards
				*(formatted++) = *(json++); i++;
			}

			// count towards
			*(formatted++) = *json; i++;
		} else
			// character visible?
			if( *json > STD_ASCII_SPACE && *json < STD_ASCII_DELETE ) { *(formatted++) = *json; i++; }
	} while( *(json++) );

	// end of formatted json
	*formatted = STD_ASCII_TERMINATOR;

	// return new json length
	return i;
}