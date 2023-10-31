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

			uint64_t value;
			uint64_t value_digits;

			// parse all characters from stream
			for( uint32_t i = 0; i < console_stream_length; ) {
				// control code?
				if( console_stream_in[ i ] == STD_ASCII_ESC ) { i++;
					// escape sequence?
					if( console_stream_in[ i ] == STD_ASCII_BRACKET_SQUARE_OPEN ) { i++;
						// check sequence number
						value = lib_string_to_integer( (uint8_t *) &console_stream_in[ i ], 10 );

						// change foreground/background color?
						if( value == 38 || value == 48 ) {
							// length of prefix in ditigs
							value_digits = lib_string_length_scope_digit( (uint8_t *) &console_stream_in[ i ] );

							// omit prefix value and separator
							i += value_digits + 1;

							// check palette type
							value = lib_string_to_integer( (uint8_t *) &console_stream_in[ i ], 10 );

							// // predefinied color palette?
							// if( value == 5 ) {
							// 	value_digits = lib_string_length_scope_digit( (uint8_t *) &console_stream_in[ i ] );

							// } // no

							// no support
							i -= value_digits + 1;
						} // no

						// display character
						lib_terminal_char( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal, STD_ASCII_BRACKET_SQUARE_OPEN );
					}

					// display control code
					lib_terminal_char( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal, STD_ASCII_CARET );
				}

				// display character
				lib_terminal_char( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal, console_stream_in[ i++ ] );
			}

			// turn on the cursor
			lib_terminal_cursor_enable( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

			// update window content on screen
			console_interface.descriptor -> flags |= STD_WINDOW_FLAG_flush;
		}
	}

	// hold the door
	while( TRUE );
}