/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_INPUT
		#include	"./input.h"
	#endif
	#ifndef	LIB_STRING
		#include	"./string.h"
	#endif

uint64_t lib_input( uint8_t *cache, uint64_t limit, uint64_t index, uint16_t key, uint8_t ctrl_semaphore ) {
	// current cache size
	uint64_t length = lib_string_length( cache );

	// select action
	switch( key ) {
		case STD_KEY_BACKSPACE: {
			// if cache is empty or has void properties
			if( ! length || ! index ) break;	// yes

			// move cursor backward
			index--;

			// move all content after cursor. one position before
			for( uint64_t i = index; i < length; i++ ) cache[ i ] = cache[ i + 1 ];

			// terminate cache in new position
			cache[ --length ] = STD_ASCII_TERMINATOR;

			// done
			break;
		}

		case STD_KEY_DELETE: {
			// if at end of cache
			if( index == length ) break;	// yes

			// move all content after cursor. one position before
			for( uint64_t i = index; i <= length; i++ ) cache[ i ] = cache[ i + 1 ];

			// done
			break;
		}

		case STD_KEY_ARROW_RIGHT: {
			// if at end of cache
			if( index == length ) break;	// yes

			// CTRL key on hold?
			if( ctrl_semaphore ) {
				// trim?
				while( index < length && cache[ index ] == STD_ASCII_SPACE ) index++;	// move cursor forward

				// until end of "word"
				while( index != length )
					// if end of "word"
					if( cache[ index ] == STD_ASCII_SPACE ) break;
					else index++;	// move cursor forward

			} else index++;	// no

			// done
			break;
		}

		case STD_KEY_ARROW_LEFT: {
			// if at beginning of cache
			if( ! index ) break;	// yes

			// CTRL key on hold?
			if( ctrl_semaphore ) {
				// trim?
				while( index && cache[ index - 1 ] == STD_ASCII_SPACE ) index--;	// move cursor backward

				// until beginning of "word"
				while( index )
					// if beginning of "word"
					if( cache[ index - 1 ] == STD_ASCII_SPACE ) break;
					else index--;	// move cursor backward

			} else index--;	// no

			// done
			break;
		}

		case STD_KEY_HOME: {
			// index at beginning of cache
			index = EMPTY;

			// done
			break;
		}

		case STD_KEY_END: {
			// index at end of cache
			index = length;

			// done
			break;
		}

		default: {
			// ignore key if CTRL is on hold
			if( ctrl_semaphore ) break;

			// cache limit acquired?
			if( length == limit ) break;	// yes

			// character printable?
			if( key < STD_ASCII_SPACE || key > STD_ASCII_TILDE ) break;	// no

			// move all cache content behind cursor, one position further
			for( uint64_t i = length; i > index; i-- ) cache[ i ] = cache[ i - 1 ];

			// insert character into cache
			cache[ index++ ] = key;

			// done
			break;
		}
	}

	// done
	return index;
}
