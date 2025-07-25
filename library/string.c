/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_STRING
		#include	"./string.h"
	#endif

uint8_t *lib_string_basename( uint8_t *path ) {
	// calculate length of provided string
	uint64_t length = lib_string_length( path );

	// TODO, do we really need that?
	// library should not care about "wrong" path provided? right?
	//
	// remove all SLASH characters from end of path
	// while( path[ length - 1 ] == '/' ) length--;

	// index of last word in path
	uint64_t i = 0;

	// search for last word in path
	for( uint64_t j = 0; j < length; j++ )
		// SLASH character found?
		if( path[ j ] == '/' ) i = j + 1;	// mark word beginning
	
	// return pointer to last word
	return (uint8_t *) &path[ i ];
}

uint8_t lib_string_compare( uint8_t *source, uint8_t *target, uint64_t length ) {
	// compare both strings
	for( uint64_t i = 0; i < length; i++ )
		// they are different?
		if( source[ i ] != target[ i ] ) return FALSE;

	// equal
	return TRUE;
}

uint64_t lib_string_count( uint8_t *string, uint64_t length, uint8_t character ) {
	// found
	uint64_t counter = 0;

	// search from the beginning
	for( uint64_t i = 0; i < length; i++ ) if( string[ i ] == character ) counter++;
	
	// return amount of specified characters
	return counter;
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

uint64_t lib_string_length_line_backward( uint8_t *string, uint64_t length ) {
	// length of string
	uint64_t n = 0;

	// increment until EMPTY
	while( *(--string) != '\n' && length-- ) n++;

	// return length of string
	return n;
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
	if( index == (uint64_t) -1 ) return 0;	// yes

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

// uint64_t lib_string_word_backward( volatile uint8_t *string, uint64_t length ) {
// 	// how far (behind) is beginning of word
// 	uint64_t i = -1;

// 	// ignore "white" characters
// 	while( length-- ) { if( string[ i ] > STD_ASCII_SPACE && string[ i ] < STD_ASCII_DELETE ) break; i--; }

// 	// search from current position
// 	while( length-- ) { if( string[ i ] <= STD_ASCII_SPACE || string[ i ] >= STD_ASCII_DELETE ) break; i--; }

// 	// word beginning at
// 	return ~i;
// }

uint64_t lib_string_word_of_letters_and_digits( uint8_t *string, uint64_t length ) {
	// search from the beginning
	for( uint64_t i = 0; i < length; i++ )
		// separator located?
		if( (string[ i ] < '0' || string[ i ] > '9') && (string[ i ] < 'a' || string[ i ] > 'z') && (string[ i ] < 'A' || string[ i ] > 'Z') ) return i;

	// the whole string is "word"
	return length;
}

uint64_t lib_string_word_end( uint8_t *string, uint64_t length, uint8_t separator ) {
	// search from the beginning
	for( uint64_t i = 0; i < length; i++ )
		// separator located?
		if( string[ i ] == separator ) return i;

	// whole string is "word"
	return length;
}

uint64_t lib_string_word_remove( uint8_t *string, uint64_t length, uint8_t separator ) {
	// remove white spaces from string
	length = lib_string_trim( string, length );

	// find first separator inside string
	uint64_t next = 0;
	while( string[ ++next ] != separator && next < length );

	// if there is no next word, return empty string
	if( next >= length ) return EMPTY;

	// move all characters inside string "next" positions back
	uint8_t i = 0;
	for( ; next < length; i++ ) string[ i ] = string[ next++ ];

	// remove white spaces from string
	i = lib_string_trim( string, i );

	// terminate string in new place
	string[ i ] = STD_ASCII_TERMINATOR;

	// return new string length (trimmed)
	return lib_string_trim( string, i );
}
