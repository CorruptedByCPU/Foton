/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_STRING
		#include	"./string.h"
	#endif

uint8_t lib_string_compare( uint8_t *source, uint8_t *target, uint64_t length ) {
	// compare both strings
	for( uint64_t i = 0; i < length; i++ )
		// they are different?
		if( source[ i ] != target[ i ] ) return FALSE;

	// equal
	return TRUE;
}

uint64_t lib_string_length( uint8_t *string ) {
	// length of string
	uint64_t length = 0;

	// increment until EMPTY
	while( *(string++) ) length++;

	// return length of string
	return length;
}

uint64_t lib_string_length_line( uint8_t *string ) {
	// length of string
	uint64_t length = 0;

	// increment until EMPTY
	while( *string != '\n' && *(string++) ) length++;

	// return length of string
	return length;
}

uint64_t lib_string_length_scope_digit( uint8_t *string ) {
	// amount of digits in string
	uint64_t length = 0;

	// increment until empty
	while( string[ 0 ] == '-' || (*string >= '0' && *string <= '9') ) { string++; length++; }

	// return string length
	return length;
}

uint64_t lib_string_to_integer( uint8_t *string, uint8_t base ) {
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

uint64_t lib_string_trim( uint8_t *string, uint64_t length ) {
	// start trimming from beginning of string
	uint64_t i = 0;

	// check beginning
	for( ; i < length; i++ )
		// printable character?
		if( string[ i ] > STD_ASCII_SPACE && string[ i ] < STD_ASCII_DELETE ) break;	// yes

	// check end
	while( i < length )
		// printable character?
		if( string[ length - 1 ] > STD_ASCII_SPACE && string[ length - 1 ] < STD_ASCII_DELETE ) break;	// yes
		// no, previous character from string
		else length--;

	// new string size
	length -= i;

	// move trimmed string at beginning
	for( uint64_t j = 0; i && j < length; j++ )
		string[ j ] = string[ j + i ];

	// return new string length
	return length;
}

uint64_t lib_string_word( uint8_t *string, uint64_t length ) {
	// search from the beginning
	for( uint64_t i = 0; i < length; i++ )
		// separator located?
		if( string[ i ] < 0x21 || string[ i ] > 0x7E ) return i;

	// the whole string is "word"
	return length;
}


uint64_t lib_string_word_end( uint8_t *string, uint64_t length, uint8_t separator ) {
	// search from the beginning
	for( uint64_t i = 0; i < length; i++ )
		// separator located?
		if( string[ i ] == separator ) return i;

	// the whole string is "word"
	return length;
}
