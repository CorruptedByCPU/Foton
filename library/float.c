/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_FLOAT
		#include	"./float.h"
	#endif
	#ifndef	LIB_INTEGER
		#include	"./integer.h"
	#endif

uint8_t *lib_float_to_string( double value, uint8_t precision ) {
	// if presision note set, use default
	if( ! precision ) precision = 2;

	// set default string area
	uint8_t *string = (uint8_t *) calloc( lib_integer_digit_count( (uint64_t) value, STD_NUMBER_SYSTEM_decimal ) + 1 + precision + 1 );

	// convert value to string
	uint64_t prefix = lib_integer_to_string( (uint64_t) value, 10, string );

	// add DOT delimiter
	string[ prefix ] = STD_ASCII_DOT;

	// number of digits after dot
	uint64_t suffix = 1;
	for( uint64_t m = 0; m < precision; m++ ) suffix *= 10;	// additional ZERO in magnitude

	// convert fraction to string
	suffix = lib_integer_to_string( (uint64_t) ((double) (value - (uint64_t) value) * (double) suffix), 10, (uint8_t *) &string[ prefix + 1 ] );

	// return string
	return string;
}