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

uint64_t lib_input( uint8_t *cache, uint64_t limit, uint64_t length, uint8_t *ctrl_semaphore ) {
	// prepare area for stream meta data
	// struct STD_STRUCTURE_STREAM_META stream_meta;

	// keep track of cursor position inside cache
	uint64_t cursor_index = length;

	// main loop
	while( TRUE ) {
		// incomming message
		uint8_t ipc_data[ STD_IPC_SIZE_byte ];
		if( std_ipc_receive( (uint8_t *) &ipc_data ) ) {
			// properties of incomming message
			struct STD_STRUCTURE_IPC_DEFAULT *ipc = (struct STD_STRUCTURE_IPC_DEFAULT *) &ipc_data;

			// message type: key?
			if( ipc -> type != STD_IPC_TYPE_keyboard ) continue;	// no

			// properties of Keyboard message
			struct STD_STRUCTURE_IPC_KEYBOARD *keyboard = (struct STD_STRUCTURE_IPC_KEYBOARD *) &ipc_data;

			// left CTRL pressed?
			if( keyboard -> key == STD_KEY_CTRL_LEFT ) *ctrl_semaphore = TRUE;
			// or released?
			if( keyboard -> key == (STD_KEY_CTRL_LEFT | STD_KEY_RELEASE) ) *ctrl_semaphore = FALSE;

			// if end of input
			if( keyboard -> key == STD_ASCII_RETURN ) {
				// remove orphaned "white" characters from input
				length = lib_string_trim( cache, length );

				// terminate command from previous one
				cache[ length ] = STD_ASCII_TERMINATOR;

				// return current cursor_index
				return length;
			}

			// remove previous character?
			if( keyboard -> key == STD_ASCII_BACKSPACE ) {
				// if input is empty
				if( ! length ) continue;	// ignore

				// remove previous character from terminal
				print( "\b" );

				// shorten input content
				length--;

				// cursor position inside cache
				cursor_index--;

				// if we weren't at end of input
				if( cursor_index != length ) {
					// move all character on right, one position back
					for( int64_t i = cursor_index; i < length; i++ ) cache[ i ] = cache[ i + 1 ];

					// preserve current cursor location
					print( "\e[s" );

					// show on terminal
					for( uint64_t i = cursor_index; i < length; i++ ) printf( "%c", cache[ i ] );

					// restore cursor location
					print( " \e[u" );
				}

				// done
				continue;
			}
		
			// remove previous character?
			if( keyboard -> key == STD_KEY_DELETE ) {
				// if input is empty or at end
				if( ! length || cursor_index == length ) continue;	// ignore

				// shorten input content
				length--;

				// move all character on right, one position back
				for( int64_t i = cursor_index; i < length; i++ ) cache[ i ] = cache[ i + 1 ];

				// preserve current cursor location
				print( "\e[s" );

				// show on terminal
				for( uint64_t i = cursor_index; i < length; i++ ) printf( "%c", cache[ i ] );

				// restore cursor location
				print( " \e[u" );

				// done
				continue;
			}

			// move cursor left?
			if( keyboard -> key == STD_KEY_ARROW_LEFT && cursor_index ) {
				// move by whole word?
				if( *ctrl_semaphore ) {
					// are we at white character?
					while( cursor_index && cache[ cursor_index - 1 ] == STD_ASCII_SPACE ) { print( "\e[D" ); cursor_index--; }

					// find beginning of previous word
					while( cursor_index ) {
						if( cache[ cursor_index - 1 ] == STD_ASCII_SPACE ) break;
					
						// move cursor left
						print( "\e[D" );

						cursor_index--;
					}
				} else {
					// yes
					print( "\e[D" );

					// cursor position inside cache
					cursor_index--;
				}
			}

			// move cursor right?
			if( keyboard -> key == STD_KEY_ARROW_RIGHT && cursor_index < length ) {
				// move by whole word?
				if( *ctrl_semaphore ) {
					// are we at white character?
					while( cursor_index < length && cache[ cursor_index ] == STD_ASCII_SPACE ) { print( "\e[C" ); cursor_index++; }

					// find end of next word
					while( cursor_index < length ) {
						if( cache[ cursor_index ] == STD_ASCII_SPACE ) break;
					
						// move cursor right
						print( "\e[C" );

						cursor_index++;
					}
				} else {
					// yes
					print( "\e[C" );

					// cursor position inside cache
					cursor_index++;
				}
			}

			// cancel operation?
			if( keyboard -> key == 'c' && *ctrl_semaphore ) {
				// move cursor at beginning of input
				while( cursor_index-- ) print( "\e[D" );

				// show input content again but with different color
				cache[ length ] = STD_ASCII_TERMINATOR;
				printf( "\e[38;5;8m%s", cache );

				return EMPTY;	// yes
			}

			// Home key?
			if( keyboard -> key == STD_KEY_HOME ) {
				// move cursor at beginning of input
				while( cursor_index ) { print( "\e[D" ); cursor_index--; }
			}

			// End key?
			if( keyboard -> key == STD_KEY_END ) {
				// move cursor at beginning of input
				while( length > cursor_index ) { print( "\e[C" ); cursor_index++; }
			}

			// if key is not printable
			if( keyboard -> key < STD_ASCII_SPACE || keyboard -> key > STD_ASCII_TILDE ) continue;	// ignore

			// input is full?
			if( length == limit ) continue;	// yes

			// insert at end of cache
			if( cursor_index == length ) {
				// insert
				cache[ length++ ] = (uint8_t) keyboard -> key;

				// show
				printf( "%c", keyboard -> key );

				// update cursor location
				cursor_index++;
			}

			// or keep between other characters
			else {
				// move all character on right, one position further
				for( int64_t i = length; i > cursor_index; i-- ) cache[ i ] = cache[ i - 1 ];

				// insert
				cache[ cursor_index++ ] = (uint8_t) keyboard -> key;

				// input string is wider
				length++;

				// show character and preserve cursor location
				printf( "%c\e[s", keyboard -> key );

				// show on terminal
				for( uint64_t i = cursor_index; i < length; i++ ) printf( "%c", cache[ i ] );

				// restore cursor location
				print( "\e[u" );
			}
		} else
			// free up AP time
			sleep( TRUE );
	}

	return EMPTY;
}