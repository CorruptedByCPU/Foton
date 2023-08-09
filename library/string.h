/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_STRING
	#define	LIB_STRING

	// returns TRUE/FALSE depending on whether strings match
	uint8_t lib_string_compare( uint8_t *source, uint8_t *target, uint64_t length );

	// returns amount of bytes until first EMPTY found
	uint64_t lib_string_length( uint8_t *string );

	// returns amount of bytes until line feed found
	uint64_t lib_string_length_line( uint8_t *string );

	// returns amount of digits inside string, until found something else than digit
	uint64_t lib_string_length_scope_digit( uint8_t *string );

	// returns value from string conversion regarded  to base, until found something else than digit
	uint64_t lib_string_to_integer( uint8_t *string, uint8_t base );

	// returns length of first printable word
	uint64_t lib_string_word( uint8_t *string, uint64_t length );

	// returns length of string before separator
	uint64_t lib_string_word_end( uint8_t *string, uint64_t length, uint8_t separator );
#endif