/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/font.h"
	#include	"../library/interface.h"
	#include	"../library/terminal.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions of Graphical User Interface
	//----------------------------------------------------------------------
	// #include	"./gui/config.h"
	#include	"./wm/config.h"
	//----------------------------------------------------------------------
	// variables, structures, console_stream_indefinitions
	//----------------------------------------------------------------------
	#include	"./console/config.h"
	//----------------------------------------------------------------------
	// variables, routines, procedures
	//----------------------------------------------------------------------
	#include	"./console/data.c"
	#include	"./console/init.c"

uint8_t console_vt100( uint8_t *string, uint64_t length ) {
	// parsed sequence length
	uint8_t sequence_length = 0;

	// control code?
	if( *(string++) != STD_ASCII_ESC ) return 0;	// no

	// escape sequence?
	if( *(string++) != STD_ASCII_BRACKET_SQUARE_OPEN ) return 0;	// no

	// start of sequence
	sequence_length += 2;

	// choose type of action
	switch( lib_string_to_integer( string, 10 ) ) {
		// reset to default?
		case 0: {
			// set default foreground/background color of terminal
			console_terminal.color_foreground = lib_color( 255 );
			console_terminal.color_background = lib_color( 232 );

			// update sequence length
			sequence_length += 1 + 1;

			// done
			break;
		}

		// change foreground color?
		case 38: {
			// move string index to next option
			string += 2 + 1;	// leave separator

			// choose color from predefinied palette?
			if( lib_string_to_integer( string, 10 ) == 5 ) {
				// update sequence length
				sequence_length += 1 + 1;	// leave separator

				// move string index to requested color
				string += 1 + 1;	// leave separator

				// set selected foreground color
				console_terminal.color_foreground = lib_color( lib_string_to_integer( string, 10 ) );

				// sequence length
				sequence_length += lib_string_length_scope_digit( string ) + 1;
			}

			// update sequence length
			sequence_length += 2 + 1;

			// done
			break;
		}

		// change background color?
		case 48: {
			// move string index to next option
			string += 2 + 1;	// leave separator

			// choose color from predefinied palette?
			if( lib_string_to_integer( string, 10 ) == 5 ) {
				// update sequence length
				sequence_length += 1 + 1;	// leave separator

				// move string index to requested color
				string += 1 + 1;	// leave separator

				// set selected foreground color
				console_terminal.color_background = lib_color( lib_string_to_integer( string, 10 ) );

				// sequence length
				sequence_length += lib_string_length_scope_digit( string ) + 1;
			}

			// update sequence length
			sequence_length += 2 + 1;

			// done
			break;
		}
	}

	// return parsed sequence length
	return sequence_length;
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize console window
	console_init();

	// main look
	while( TRUE ) {
		// end of shell?
		if( ! std_pid_check( console_pid_of_shell ) ) std_exit();	// quit from console too

		// incomming message
		uint8_t data[ STD_IPC_SIZE_byte ];
		if( std_ipc_receive( (uint8_t *) &data ) )
			// send this message forward to child
			std_ipc_send( console_pid_of_shell, (uint8_t *) &data );

		// get data from input stream
		uint64_t console_stream_length = std_stream_in( console_stream_in );

		// if there is incomming stream
		if( console_stream_length ) {
			// disable cursor, no CPU power waste
			lib_terminal_cursor_disable( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

			// parse all characters from stream
			for( uint32_t i = 0; i < console_stream_length; ) {
				// sequence length
				uint8_t sequence_length = console_vt100( (uint8_t *) &console_stream_in[ i ], console_stream_length - i );

				// display character
				if( ! sequence_length ) lib_terminal_char( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal, console_stream_in[ i++ ] );

				// move forward of sequence
				i += sequence_length;
			}

			// turn on the cursor
			lib_terminal_cursor_enable( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

			// update window content on screen
			console_interface.descriptor -> flags |= STD_WINDOW_FLAG_flush;

			// set new meta properties of stream
			console_stream_meta.x = console_terminal.cursor_x;
			console_stream_meta.y = console_terminal.cursor_y;

			// update metadata of input stream
			std_stream_set( (uint8_t *) &console_stream_meta, STD_STREAM_IN );
		}
	}

	// hold the door
	while( TRUE );
}