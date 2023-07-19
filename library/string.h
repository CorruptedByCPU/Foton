/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_STRING
	#define	LIB_STRING

	// returns amount of bytes until first EMPTY found
	uint64_t lib_string_length( uint8_t *string );

	// returns amount of digits inside string, until found something else than digit
	uint64_t lib_string_length_scope_digit( const char *string );

	// returns value from string conversion regarded  to base, until found something else than digit
	uint64_t lib_string_to_integer( const char *string, uint8_t base );
#endif