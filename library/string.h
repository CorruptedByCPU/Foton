/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_STRING
	#define	LIB_STRING

	// returns pointer to last word in path
	uint8_t *lib_string_basename( uint8_t *path );

	// returns TRUE/FALSE depending on whether strings match
	uint8_t lib_string_compare( uint8_t *source, uint8_t *target, uint64_t length );

	// returns amount of selected ASCII character
	uint64_t lib_string_count( uint8_t *string, uint64_t length, uint8_t character );

	// returns amount of bytes until first EMPTY found
	uint64_t lib_string_length( uint8_t *string );

	// returns amount of bytes until line feed found
	uint64_t lib_string_length_line( uint8_t *string );

	// returns amount of digits inside string, until found something else than digit
	uint64_t lib_string_length_scope_digit( uint8_t *string );

	// returns value from string conversion regarded  to base, until found something else than digit
	uint64_t lib_string_to_integer( uint8_t *string, uint8_t base );

	// returns new length of string after trimming
	uint64_t lib_string_trim( uint8_t *string, uint64_t length );

	// returns length of first printable word
	uint64_t lib_string_word( uint8_t *string, uint64_t length );

	// returns length of string before separator
	uint64_t lib_string_word_end( uint8_t *string, uint64_t length, uint8_t separator );

	// returns length of first word made from alphabet or digits
	uint64_t lib_string_word_of_letters_and_digits( uint8_t *string, uint64_t length );

	// remove from string first word occurence, return new string length (trimmed)
	uint64_t lib_string_word_remove( uint8_t *string, uint64_t length, uint8_t separator );
#endif