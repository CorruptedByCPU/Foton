/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_INTEGER
		#include	"./integer.h"
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

	// area obtained?
	if( ! target ) return EMPTY;	// no
	
	// set metadata of area
	*target = MACRO_PAGE_ALIGN_UP( byte + STD_ALLOC_METADATA_byte ) - STD_ALLOC_METADATA_byte;

	// return allocated area pointer
	return target + (STD_ALLOC_METADATA_byte >> STD_SHIFT_8);
}

void *realloc( void *source, size_t byte ) {
	// move pointer to metadata area
	uint64_t *ptr = (uint64_t *) MACRO_PAGE_ALIGN_DOWN( (uintptr_t) source );

	// calculate length of areas in Pages
	uint64_t offset = MACRO_PAGE_ALIGN_UP( *ptr + STD_ALLOC_METADATA_byte );
	uint64_t limit = MACRO_PAGE_ALIGN_UP( byte + STD_ALLOC_METADATA_byte );

	// do we need wider area?
	if( byte > *ptr ) {	// yes
		// alloc new area
		uint64_t *target = (uint64_t *) malloc( byte );

		// wider area obtained?
		if( ! target ) return EMPTY;	// no
		
		// copy content
		memcpy( (uint8_t *) target, (uint8_t *) source, (uint64_t) *ptr );

		// release old area
		free( source );

		// return pointer to new allocated area
		return target;
	}

	// shrink area if possible
	if( offset > limit ) { std_memory_release( (uintptr_t) source + limit, (offset - limit) >> STD_SHIFT_PAGE ); *ptr = MACRO_PAGE_ALIGN_UP( byte + STD_ALLOC_METADATA_byte ) - STD_ALLOC_METADATA_byte; }

	// no
	return source;
}

void *calloc( size_t byte ) {
	// assign place for area of definied size
	uint8_t *area = (uint8_t *) malloc( byte );

	// area obtained?
	if( ! area ) return EMPTY;	// no
	
	// clean up area
	for( uint64_t i = 0; i < byte; i++ ) area[ i ] = EMPTY;

	// return allocated area pointer
	return (void *) area;
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

double minf( double first, double second ) {
	if( first < second ) return first;
	else return second;
}

double maxf( double first, double second ) {
	if( first > second ) return first;
	else return second;
}

void log( const char *string, ... ) {
	// properties of argument list
	va_list argv;

	// start of argument list
	va_start( argv, string );

	// cache for values
	uint8_t digits[ 64 ];

	// for every character from string
	uint64_t length = lib_string_length( (uint8_t *) string );
	for( uint64_t i = 0; i < length; i++ ) {
		// special character?
		if( string[ i ] == '%' ) {	
			// prefix before type?
			uint64_t prefix = lib_string_length_scope_digit( (uint8_t *) &string[ ++i ] );
			uint64_t p_value = lib_string_to_integer( (uint8_t *) &string[ i ], 10 );

			// omit prefix value if existed
			i += prefix;

			// definied suffix length?
			uint64_t pre_suffix = EMPTY;
			if( string[ i ] == '.' && string[ i + 1 ] == '*' ) {
				// amount of digits after digit delimiter
				pre_suffix = va_arg( argv, uint64_t );

				// leave predefinied suffix
				i += 2;
			}

			// check sequence type
			switch( string[ i ] ) {
				case '%': {
					// just show '%' character
					break;
				}

				case 'b': {
					// retrieve value
					uint64_t value = va_arg( argv, uint64_t );

					// show 'value' on terminal
					std_log( (uint8_t *) &digits, lib_integer_to_string( value, 2, (uint8_t *) &digits ) );

					// next character from string
					continue;
				}

				case 'c': {
					// retrieve character
					uint8_t c = va_arg( argv, uint64_t );
					
					// show 'character' on terminal
					std_log( (uint8_t *) &c, 1 );

					// next character from string
					continue;
				}

				case 'd': {
					// retrieve value
					uint64_t value = va_arg( argv, uint64_t );

					// value signed?
					if( value & 0x8000000000000000 ) {
						// show 'character' on terminal
						std_log( (uint8_t *) "-", 1 );

						// remove sign
						value = ~value + 1;
					}

					// show 'value' on terminal
					std_log( (uint8_t *) &digits, lib_integer_to_string( value, 10, (uint8_t *) &digits ) );

					// next character from string
					continue;
				}

				case 's': {
					// string properties
					uint8_t *substring = va_arg( argv, uint8_t * );
					uint64_t length = lib_string_length( substring );

					// change string length if predefinied value exist
					if( pre_suffix ) length = pre_suffix;

					// show 'substring' on terminal
					std_log( substring, length );

					// next character from string
					continue;
				}

				case 'u': {
					// retrieve value
					uint64_t value = va_arg( argv, uint64_t );

					// show 'value' on terminal
					std_log( (uint8_t *) &digits, lib_integer_to_string( value, 10, (uint8_t *) &digits ) );

					// next character from string
					continue;
				}

				case 'X': {
					// retrieve value
					uint64_t value = va_arg( argv, uint64_t );

					// show 'value' on terminal
					std_log( (uint8_t *) &digits, lib_integer_to_string( value, 16, (uint8_t *) &digits ) );

					// next character from string
					continue;
				}
			}
		}

		// no, show it
		std_log( (uint8_t *) &string[ i ], 1 );
	}

	// end of arguemnt list
	va_end( argv );
}

void print( const char *string ) {
	// send to default output
	std_stream_out( (uint8_t *) string, lib_string_length( (uint8_t *) string ) );
}

void putc( uint8_t character ) {
	// send to default output
	std_stream_out( (uint8_t *) &character, 1 );
}

void printf( const char *string, ... ) {
	// properties of argument list
	va_list argv;

	// start of argument list
	va_start( argv, string );

	// cache for values
	uint8_t digits[ 64 ];

	// alloc area for output string and its index
	uint64_t c = 0;
	uint8_t *cache = (uint8_t *) malloc( EMPTY );

	// for every character from string
	uint64_t length = lib_string_length( (uint8_t *) string );
	for( uint64_t s = 0; s < length; s++ ) {
		// special character?
		if( string[ s ] == '%' ) {	
			// prefix before type?
			uint64_t prefix = lib_string_length_scope_digit( (uint8_t *) &string[ ++s ] );
			uint64_t p_value = lib_string_to_integer( (uint8_t *) &string[ s ], 10 );

			// omit prefix value if existed
			s += prefix;

			// definied suffix length?
			uint64_t pre_suffix = EMPTY;
			if( string[ s ] == '.' && string[ s + 1 ] == '*' ) {
				// amount of digits after digit delimiter
				pre_suffix = va_arg( argv, uint64_t );

				// leave predefinied suffix
				s += 2;
			}

			// check sequence type
			switch( string[ s ] ) {
				case '%': {
					// just show '%' character
					break;
				}

				case 'c': {
					// resize cache for substring
					cache = (uint8_t *) realloc( cache, c + 1 );

					// insert substring into cache
					cache[ c++ ] = va_arg( argv, uint64_t );

					// next character from string
					continue;
				}

				case '.':
				case 'f': {
					// retrieve value
					double value = va_arg( argv, double );

					// convert value to string
					uint8_t v_digits = lib_integer_to_string( (uint64_t) value, 10, (uint8_t *) &digits );

					// align
					while( p_value-- > v_digits ) {
						// insert space before value
						cache = (uint8_t *) realloc( cache, c + 1 );
						cache[ c++ ] = STD_ASCII_SPACE;
					}

					// resize cache for value
					cache = (uint8_t *) realloc( cache, c + v_digits );

					// insert prefix of value on cache
					for( uint8_t i = 0; i < v_digits; i++ ) cache[ c++ ] = digits[ i ];

					// resize cache for delimiter
					cache = (uint8_t *) realloc( cache, c + 1 );

					// add DOT delimiter
					cache[ c++ ] = STD_ASCII_DOT;

					// amount of digits after digit delimiter
					uint64_t suffix = lib_string_length_scope_digit( (uint8_t *) &string[ ++s ] );
					uint64_t s_value = lib_string_to_integer( (uint8_t *) &string[ s ], 10 );

					// number of digits after dot
					uint64_t s_digits = 1;	// fraction magnitude
					if( s_value ) for( uint8_t m = 0; m < s_value; m++ ) s_digits *= 10;	// additional ZERO in magnitude
					else s_digits = 1000000;	// if not specified set default

					// convert fraction to string
					uint8_t f_digits = lib_integer_to_string( (uint64_t) ((double) (value - (uint64_t) value) * (double) s_digits), 10, (uint8_t *) &digits );

					// resize cache for fraction
					cache = (uint8_t *) realloc( cache, c + f_digits );

					// insert suffix of value on cache
					for( uint8_t i = 0; i < f_digits; i++ ) cache[ c++ ] = digits[ i ];

					// align
					while( s_value-- > f_digits ) {
						// insert space after fraction
						cache = (uint8_t *) realloc( cache, c + 1 );
						cache[ c++ ] = STD_ASCII_DIGIT_0;
					}

					// omit suffix value if existed
					s += suffix;

					// next character from string
					continue;
				}

				case 's': {
					// string properties
					uint8_t *substring = va_arg( argv, uint8_t * );
					uint64_t length = lib_string_length( substring );

					// change string length if predefinied value exist
					if( pre_suffix ) length = pre_suffix;

					// resize cache for substring
					cache = (uint8_t *) realloc( cache, c + length );

					// insert substring into cache
					for( uint64_t i = 0; i < length; i++ ) cache[ c++ ] = substring[ i ];

					// next character from string
					continue;
				}

				case 'u': {
					// retrieve value
					uint64_t value = va_arg( argv, uint64_t );

					// convert value to string
					uint8_t v = lib_integer_to_string( value, 10, (uint8_t *) &digits );

					// set prefix before value if higher than value ifself
					while( p_value-- > v ) {
						// insert prefix before value
						cache = (uint8_t *) realloc( cache, c + 1 );
						cache[ c++ ] = STD_ASCII_SPACE;
					}

					// insert valuse string into cache
					cache = (uint8_t *) realloc( cache, c + v );
					for( uint8_t i = 0; i < v; i++ ) cache[ c++ ] = digits[ i ];

					// next character from string
					continue;
				}

				case 'd': {
					// retrieve value
					uint64_t value = va_arg( argv, uint64_t );

					// signed?
					if( value & STD_SIZE_QWORD_sign ) { value = ~value + 1; print( "-" ); }

					// convert value to string
					uint8_t v = lib_integer_to_string( value, 10, (uint8_t *) &digits );

					// set prefix before value if higher than value ifself
					while( p_value-- > v ) {
						// insert prefix before value
						cache = (uint8_t *) realloc( cache, c + 1 );
						cache[ c++ ] = STD_ASCII_SPACE;
					}

					// insert valuse string into cache
					cache = (uint8_t *) realloc( cache, c + v );
					for( uint8_t i = 0; i < v; i++ ) cache[ c++ ] = digits[ i ];

					// next character from string
					continue;
				}

				case 'X': {
					// retrieve value
					uint64_t value = va_arg( argv, uint64_t );

					// convert value to string
					uint8_t v = lib_integer_to_string( value, 16, (uint8_t *) &digits );

					// set prefix before value if higher than value ifself
					while( p_value-- > v ) {
						// insert prefix before value
						cache = (uint8_t *) realloc( cache, c + 1 );
						cache[ c++ ] = STD_ASCII_DIGIT_0;
					}

					// insert valuse string into cache
					cache = (uint8_t *) realloc( cache, c + v );
					for( uint8_t i = 0; i < v; i++ ) cache[ c++ ] = digits[ i ];

					// next character from string
					continue;
				}
			}
		}

		// insert character into cache
		cache = (uint8_t *) realloc( cache, c + 1 );
		cache[ c++ ] = string[ s ];
	}

	// send prepared string to default output
	std_stream_out( cache, c );

	// release cached string
	free( cache );

	// end of arguemnt list
	va_end( argv );
}

void sprintf( const char *string, ... ) {
	// properties of argument list
	va_list argv;

	// start of argument list
	va_start( argv, string );

	// cache for values
	uint8_t digits[ 64 ];

	// area for output string and its index
	uint64_t c = 0;
	uint8_t *cache = va_arg( argv, uint8_t * );

	// for every character from string
	uint64_t length = lib_string_length( (uint8_t *) string );
	for( uint64_t s = 0; s < length; s++ ) {
		// special character?
		if( string[ s ] == '%' ) {	
			// prefix before type?
			uint64_t prefix = lib_string_length_scope_digit( (uint8_t *) &string[ ++s ] );
			uint64_t p_value = lib_string_to_integer( (uint8_t *) &string[ s ], 10 );

			// omit prefix value if existed
			s += prefix;

			// definied suffix length?
			uint64_t pre_suffix = EMPTY;
			if( string[ s ] == '.' && string[ s + 1 ] == '*' ) {
				// amount of digits after digit delimiter
				pre_suffix = va_arg( argv, uint64_t );

				// leave predefinied suffix
				s += 2;
			}

			// check sequence type
			switch( string[ s ] ) {
				case '%': {
					// just show '%' character
					break;
				}

				case 'c': {
					// resize cache for substring
					cache = (uint8_t *) realloc( cache, c + 1 );

					// insert substring into cache
					cache[ c++ ] = va_arg( argv, uint64_t );

					// next character from string
					continue;
				}

				case 's': {
					// string properties
					uint8_t *substring = va_arg( argv, uint8_t * );
					uint64_t length = lib_string_length( substring );

					// change string length if predefinied value exist
					if( pre_suffix ) length = pre_suffix;

					// insert substring into cache
					for( uint64_t i = 0; i < length; i++ ) cache[ c++ ] = substring[ i ];

					// next character from string
					continue;
				}

				case 'u': {
					// retrieve value
					uint64_t value = va_arg( argv, uint64_t );

					// convert value to string
					uint8_t v = lib_integer_to_string( value, 10, (uint8_t *) &digits );

					// set prefix before value if higher than value ifself
					while( p_value-- > v ) {
						// insert prefix before value
						cache[ c++ ] = STD_ASCII_SPACE;
					}

					// insert valuse string into cache
					for( uint8_t i = 0; i < v; i++ ) cache[ c++ ] = digits[ i ];

					// next character from string
					continue;
				}
			}
		}

		// insert character into cache
		cache[ c++ ] = string[ s ];
	}

	// end of arguemnt list
	va_end( argv );
}

uint64_t pow( uint64_t base, uint64_t exponent ) {
	// resulf by default
	uint64_t result = 1;

	// until exponent exist
	while( exponent ) {
		// calculate
		if( (exponent & 1) == 1) result *= base;

		// remove exponent fraction
		exponent >>= 1;

		// change power of
		base *= base;
	}

	// return result
	return result;
}

uint16_t getkey( void ) {
	// incomming message
	uint8_t ipc_data[ STD_IPC_SIZE_byte ];
	if( ! std_ipc_receive_by_type( (uint8_t *) &ipc_data, STD_IPC_TYPE_keyboard ) ) return EMPTY;	// nothing

	// properties of Keyboard message
	struct STD_STRUCTURE_IPC_KEYBOARD *keyboard = (struct STD_STRUCTURE_IPC_KEYBOARD *) &ipc_data;

	// return received key
	return keyboard -> key;
}

void exit( void ) {
	// execute leave out routine
	__asm__ volatile( "" :: "a" (STD_SYSCALL_EXIT) );
	std_syscall_empty();
}

FILE *fopen( uint8_t *path ) {
	// assign area for file structure
	FILE *file = malloc( sizeof( FILE ) );

	// open new socket for file
	file -> socket = std_file_open( path, lib_string_trim( path, lib_string_length( path ) ) );

	// if file doesn't exist
	if( file -> socket < 0 ) {
		// release file pointer
		free( file );

		// cannot open such file
		return EMPTY;
	}

	// retrieve properties of opened file
	std_file( file );

	// return all file properties
	return file;
}

void fclose( FILE *file ) {
	// apply all changes and close
	std_file_close( file -> socket );

	// release file structure
	free( file );
}

void fread( FILE *file, uint8_t *cache, uint64_t byte ) {
	// read N Bytes into provided cache
	std_file_read( file, cache, byte );
}

void fwrite( FILE *file, uint8_t *cache, uint64_t byte ) {
	// write N Bytes from provided cache to file
	std_file_write( file, cache, byte );
}

FILE *touch( uint8_t *path, uint8_t type ) {
	// assign area for file structure
	FILE *file = malloc( sizeof( FILE ) );

	// open new socket for file
	file -> socket = std_file_touch( path, type );

	// if cannot create file
	if( file -> socket < 0 ) {
		// release file pointer
		free( file );

		// cannot create such file
		return EMPTY;
	}

	// retrieve properties of opened file
	std_file( file );

	// return all file properties
	return file;
}

void sleep( uint64_t ms ) {
	// set release in future
	uint64_t wait = std_microtime() + ms;

	// release CPU time until we are ready
	while( wait > std_microtime() ) __asm__ volatile( "int $0x40" );
}