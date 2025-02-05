/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_INTEGER
		#include	"./integer.h"
	#endif

uint8_t lib_integer_digit_count( uint64_t value, uint8_t base ) {
	// if unsupported number system
	if( base < 2 ) return EMPTY;	// return ZERO

	// number in digits
	uint8_t digits = 0;

	// parse every digit from value
	while( value ) {
		// even 
		digits++;

		// remove digit from value
		value /= base;
	}

	// return number of digits
	if( ! digits ) return digits + 1;
	return digits;
}

// length of "string" must be 64 characters
uint8_t lib_integer_to_string( uint64_t value, uint8_t base, uint8_t *string ) {
	// if unsupported number system
	if( base < 2 || base > 36 ) return EMPTY;	// return EMPTY

	// length of string
	uint8_t length = lib_integer_digit_count( value, base );

	// value equal to ZERO?
	if( ! value ) { string[ value ] = '0'; return length; }

	// digit index on string
	uint8_t index = 0;

	// parse every digit from value
	while( value ) {
		// prepare place for digit
		index++;

		// store first digit from value
		string[ length - index ] = (value % base) | '0';

		// if digit is alfanumerical
		if( string[ length - index ] > '9' ) string[ length - index ] += 0x07;	// ASCII "A"

		// remove digit from value
		value /= base;
	}

	// return string length
	return length;
}

size_t lib_integer_limit_unsigned( uint8_t byte ) {
	switch( byte ) {
		case 2: {
			// 65 536
			return UINT16_MAX;
		}
	}

	// default for 1 Byte
	return 256;
}
