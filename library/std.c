/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_STD
		#include	"./std.h"
	#endif
	#ifndef	LIB_STRING
		#include	"./string.h"
	#endif

	//----------------------------------------------------------------------
	// std routines, procedures
	//----------------------------------------------------------------------
	#include	"./std/syscall.c"

	//------------------------------------------------------------------------------
	// substitute of libc
	//------------------------------------------------------------------------------

	// multiples of 0x10, but not less than 0x10
	#define	STD_ALLOC_METADATA_byte	0x10

void *malloc( size_t byte ) {
	// assign place for area of definied size
	uint64_t *target = (uint64_t *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( byte + STD_ALLOC_METADATA_byte ) >> STD_SHIFT_PAGE );

	// set metadata of area
	*target = MACRO_PAGE_ALIGN_UP( byte + STD_ALLOC_METADATA_byte ) - STD_ALLOC_METADATA_byte;

	// return allocated area pointer
	return target + (STD_ALLOC_METADATA_byte >> STD_SHIFT_8);
}

void *realloc( void *source, size_t byte ) {
	// move pointer to metadata area
	uint64_t *ptr = (uint64_t *) MACRO_PAGE_ALIGN_DOWN( (uintptr_t) source );

	// do we need wider area?
	if( byte > *ptr ) {	// yes
		// alloc new area
		uint64_t *target = (uint64_t *) malloc( byte );

		// copy content
		memcpy( (uint8_t *) target, (uint8_t *) source, (uint64_t) *ptr );

		// release old area
		free( source );

		// return pointer to new allocated area
		return target;
	}
	
	// no
	return source;
}

void free( void *source ) {
	// move pointer to metadata area
	uint64_t *ptr = (uint64_t *) MACRO_PAGE_ALIGN_DOWN( (uintptr_t) source );

	// release assigned area
	std_memory_release( (uintptr_t) ptr, MACRO_PAGE_ALIGN_UP( *ptr + STD_ALLOC_METADATA_byte ) >> STD_SHIFT_PAGE );
}

double strtof( uint8_t *string, uint64_t length ) {
	// check if value is signed
	int8_t mark = FALSE;	// by default no
	if( string[ 0 ] == '-' ) { mark = TRUE; ++string; }	// yes

	// amount of digits before dot precision
	uint64_t prefix_length = lib_string_length_scope_digit( string );
	// and value representing that digits
	int64_t prefix_value = lib_string_to_integer( string, 10 );
	// keep value as double point
	double prefix = (double) prefix_value;

	// move string pointer after dot precision
	string += prefix_length + 1;

	// amount of digits after dot precision
	uint64_t result_length = lib_string_length_scope_digit( string );
	// and value representing that digits
	int64_t result_value = lib_string_to_integer( string, 10 );
	// keep value as double point
	double result = (double) result_value;

	// move all digits of result to behind dot precision if not ZERO
	if( result_value ) while( result_length-- ) result /= 10.f;

	// connect prefix and result regarded of sign mark
	result += prefix;
	if( mark ) result = -result;

	// return double point value
	return result;
}

uint64_t abs( int64_t i ) {
	// return absolute value of
	return i < 0 ? -i : i;
}

double fmod( double x, double y ) {
	// return modulo of
	return x - (int64_t) (x / y) * y;
}

float sqrtf( float x ) {
	union {
		int64_t	i;
		float	x;
	} u;

	u.x = x;
	u.i = (1 << 29) + (u.i >> 1) - (1 << 22);

	u.x = u.x + x / u.x;
	u.x = 0.25f * u.x + x / u.x;

	return u.x;
}

double maxf( double first, double second ) {
	if( first > second ) return first;
	else return second;
}