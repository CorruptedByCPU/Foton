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

void *malloc( size_t byte ) {
	// assign place for area of definied size
	uint64_t *target = (uint64_t *) std_memory_alloc( byte + 0x10 );

	// set metadata of area
	*target = MACRO_PAGE_ALIGN_UP( byte + 0x10 ) >> STD_SHIFT_PAGE;

	// return allocated area pointer
	return target + 0x02;
}

void *realloc( void *source, size_t byte ) {
	// move pointer to metadata area
	uint64_t *ptr = (uint64_t *) MACRO_PAGE_ALIGN_DOWN( (uintptr_t) source );

	// do we need wider area?
	if( byte > (*ptr << STD_SHIFT_PAGE) ) {	// yes
		// alloc new area
		uint64_t *target = (uint64_t *) malloc( byte );

		// copy content
		memcpy( (uint8_t *) source, (uint8_t *) target, byte );

		// release old area
		free( source );

		// return pointer to new allocated area
		return target;
	} else	// no
		return source;
}

void free( void *source ) {
	// move pointer to metadata area
	uint64_t *ptr = (uint64_t *) MACRO_PAGE_ALIGN_DOWN( (uintptr_t) source );

	// release assigned area
	std_memory_release( (uintptr_t) ptr, *ptr );
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