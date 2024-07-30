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

uint64_t lib_input( uint8_t *cache, uint64_t length_max, uint64_t length_current, uint8_t wrapable, uint8_t *ctrl_semaphore ) {
	// prepare area for stream meta data
	// struct STD_STREAM_STRUCTURE_META stream_meta;

	// main loop
	while( TRUE ) {
		// incomming message
		uint8_t ipc_data[ STD_IPC_SIZE_byte ];
		if( std_ipc_receive( (uint8_t *) &ipc_data ) ) {
			// properties of incomming message
			struct STD_IPC_STRUCTURE_DEFAULT *ipc = (struct STD_IPC_STRUCTURE_DEFAULT *) &ipc_data;

			// message type: key?
			if( ipc -> type != STD_IPC_TYPE_keyboard ) continue;	// no

			// properties of Keyboard message
			struct STD_IPC_STRUCTURE_KEYBOARD *keyboard = (struct STD_IPC_STRUCTURE_KEYBOARD *) &ipc_data;

			// left CTRL pressed?
			if( keyboard -> key == STD_KEY_CTRL_LEFT ) *ctrl_semaphore = TRUE;
			// or released?
			if( keyboard -> key == (STD_KEY_CTRL_LEFT | STD_KEY_RELEASE) ) *ctrl_semaphore = FALSE;

			// if end of input
			if( keyboard -> key == STD_ASCII_RETURN ) {
				// remove orphaned "white" characters from input
				length_current = lib_string_trim( cache, length_current );

				// terminate command from previous one
				cache[ length_current ] = STD_ASCII_TERMINATOR;

				// return current length
				return length_current;
			}

			// remove previous character?
			if( keyboard -> key == STD_ASCII_BACKSPACE ) {
				// if input is empty
				if( ! length_current ) continue;	// ignore

				// remove previous character from terminal
				print( "\b" );

				// shorten input content
				length_current--;

				// done
				continue;
			}

			// cancel operation?
			if( keyboard -> key == 'c' && *ctrl_semaphore ) return EMPTY;	// yes

			// if key is not printable
			if( keyboard -> key < STD_ASCII_SPACE || keyboard -> key > STD_ASCII_TILDE ) continue;	// ignore

			// input is full?
			if( length_current + 1 >= length_max ) continue;	// yes

			// preserve character inside input
			cache[ length_current++ ] = (uint8_t) keyboard -> key;

			// show on terminal
			print( (const char *) &keyboard -> key );
		} else
			// free up AP time
			sleep( TRUE );
	}

	return EMPTY;
}