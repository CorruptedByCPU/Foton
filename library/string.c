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

uint64_t lib_string_length_scope_digit( const char *string ) {
	// amount of digits in string
	uint64_t length = 0;

	// increment until empty
	while( string[ 0 ] == '-' || (*string >= '0' && *string <= '9') ) { string++; length++; }

	// return string length
	return length;
}

uint64_t lib_string_to_integer( const char *string, uint8_t base ) {
	// calculated value
	uint64_t value = EMPTY;

	// if unsupported number system
	if( base < 2 || base > 36 ) return 0;	// return ZERO

	// amount of digits to convert
	uint64_t index = lib_string_length_scope_digit( string );

	// empty string?
	if( index == -1 ) return 0;	// yes

	// first order of magnitude power
	uint64_t order_of_magnitude = 1;

	// calulate
	while( index != EMPTY ) {
		// retrieve digit from value
		uint8_t digit = string[ --index ];

		// last digit is a "sign"?
		if( digit == '-' && base == 10 )
			// return value with sign
			return ~value + 1;

		// digit of supported base?
		if( digit >= '0' && digit <= '9') {
			// fix digit
			digit -= '0';
		}
		else if( digit >= 'A' && digit <= 'Z' ) {
			// fix digit
			digit -= '0' + ('A' - '9');
		}
		else if( digit >= 'a' && digit <= 'z' ) {
			// fix digit
			digit -= '0' + ('a' - '0');
		}
		else return 0;	// invalid digit base

		// calculate value of the first order of magnitude
		value += digit * order_of_magnitude;

		// next order of magnitude
		order_of_magnitude *= base;
	}

	// return value
	return value;
}