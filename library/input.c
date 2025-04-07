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

uint64_t lib_input( struct LIB_INPUT_STRUCTURE *input, uint8_t *cache, uint64_t limit, uint64_t length, uint8_t *ctrl_semaphore ) {
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

				// history available?
				if( input ) {
					// do not preserve duplicates
					uint64_t length_previous = lib_string_length_line_previous( input -> history + input -> history_index - TRUE, input -> history_index - TRUE );
					if( ! lib_string_compare( (uint8_t *) &input -> history[ input -> history_index - (length_previous + TRUE) ], cache, length ) ) {
						// keep in history
						input -> history = (uint8_t *) realloc( input -> history, input -> history_length + length + 1 );
						for( uint64_t i = 0; i < length; i++ ) input -> history[ input -> history_length++ ] = cache[ i ];

						// separate each command with new line character
						input -> history[ input -> history_length++ ] = STD_ASCII_NEW_LINE;

						// reset history index
						input -> history_index = input -> history_length;
					}
				}

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
		
			// remove next character?
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

						// cursor position inside cache
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
					while( cursor_index < length && cache[ cursor_index ] == STD_ASCII_SPACE ) {
						// move cursor right
						print( "\e[C" );

						// cursor position inside cache
						cursor_index++;
					}

					// find end of next word
					while( cursor_index < length ) {
						if( cache[ cursor_index ] == STD_ASCII_SPACE ) break;
					
						// move cursor right
						print( "\e[C" );

						// cursor position inside cache
						cursor_index++;
					}
				} else {
					// yes
					print( "\e[C" );

					// cursor position inside cache
					cursor_index++;
				}
			}

			// select previous command?
			if( keyboard -> key == STD_KEY_ARROW_UP && input ) {
				// beginning of history?
				if( ! input -> history_index ) continue;	// ignore

				// calculate length of previous command
				uint64_t length_previous = lib_string_length_line_previous( input -> history + input -> history_index - TRUE, input -> history_index - TRUE );
				input -> history_index -= length_previous + TRUE;

				// move cursor at end of current command
				if( cursor_index != length ) while( cursor_index++ != length ) { print( "\e[C" ); cursor_index++; }

				// and clear prompt
				while( length-- ) { print( "\b" ); cursor_index--; }

				// load previous command
				for( uint64_t i = 0; i < length_previous; i++ ) cache[ i ] = input -> history[ input -> history_index + i ]; cache[ length_previous ] = STD_ASCII_TERMINATOR;

				// show it
				printf( "%s", cache );
				cursor_index = length = length_previous;
			}

			// select previous command?
			if( keyboard -> key == STD_KEY_ARROW_DOWN && input ) {
				// end of history?
				if( input -> history_index == input -> history_length ) continue;	// ignore

				// calculate length of next command
				input -> history_index += lib_string_length_line( input -> history + input -> history_index ) + TRUE;
				uint64_t length_next = lib_string_length_line( input -> history + input -> history_index );

				// move cursor at end of current command
				if( cursor_index != length ) while( cursor_index++ != length ) { print( "\e[C" ); cursor_index++; }

				// and clear prompt
				while( length-- ) { print( "\b" ); cursor_index--; }

				// load next command
				for( uint64_t i = 0; i < length_next; i++ ) cache[ i ] = input -> history[ input -> history_index + i ]; cache[ length_next ] = STD_ASCII_TERMINATOR;

				// show it
				printf( "%s", cache );
				cursor_index = length = length_next;
			}

			// cancel operation?
			if( keyboard -> key == 'c' && *ctrl_semaphore ) {
				// move cursor at beginning of input
				while( cursor_index-- ) print( "\e[D" );

				// show input content again but with different color
				cache[ length ] = STD_ASCII_TERMINATOR;
				printf( "\e[38;5;8m%s", cache );

				// yes
				return EMPTY;
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

struct LIB_INPUT_STRUCTURE *lib_input_init( void ) {
	// allocate memory for input structure
	struct LIB_INPUT_STRUCTURE *input = (struct LIB_INPUT_STRUCTURE *) std_memory_alloc( TRUE );

	// allocate memory for input history
	input -> history = (uint8_t *) malloc( TRUE );
	input -> history_index = EMPTY;
	input -> history_length = EMPTY;

	// return input structure pointer
	return input;
}

uint64_t lib_input_not_interactive( uint8_t *cache, uint64_t limit, uint64_t index, uint16_t key, uint8_t ctrl_semaphore ) {
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
