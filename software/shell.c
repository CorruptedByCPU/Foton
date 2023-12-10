/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/string.h"
	//----------------------------------------------------------------------
	// variables, structures, console_stream_indefinitions
	//----------------------------------------------------------------------
	#include	"./shell/config.h"
	//----------------------------------------------------------------------
	// variables, routines, procedures
	//----------------------------------------------------------------------
	#include	"./shell/data.c"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// assign area for command prompt
	shell_command = (uint8_t *) malloc( SHELL_COMMAND_limit + 1 );

	// new prompt loop
	while( TRUE ) {
		// retrieve stream meta data
		struct STD_STREAM_STRUCTURE_META stream_meta;
		while( ! std_stream_get( (uint8_t *) &stream_meta, STD_STREAM_OUT ) );

		// cursor at beginning of line?
		if( stream_meta.x ) print( "\n" );	// no, move cursor to next line

		// show prompt
		print( "\e[38;5;47m$\e[0m " );

		// length of string passed by user
		uint64_t shell_command_length = EMPTY;

		// readline
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

				// remember state of special behavior - key, or take action immediately
				switch( keyboard -> key ) {
					// left alt pressed
					case STD_KEY_ALT_LEFT: { shell_keyboard_status_alt_left = TRUE; break; }

					// left alt released
					case STD_KEY_ALT_LEFT | 0x80: { shell_keyboard_status_alt_left = FALSE; break; }

					// shift left pressed
					case STD_KEY_SHIFT_LEFT: { shell_keyboard_status_shift_left = TRUE; break; }
				
					// shift left released
					case STD_KEY_SHIFT_LEFT | 0x80: { shell_keyboard_status_shift_left = FALSE; break; }

					// shift left pressed
					case STD_KEY_CTRL_LEFT: { shell_keyboard_status_ctrl_left = TRUE; break; }
				
					// shift left released
					case STD_KEY_CTRL_LEFT | 0x80: { shell_keyboard_status_ctrl_left = FALSE; break; }
				}

				// if end of line
				if( keyboard -> key == STD_ASCII_RETURN ) {
					// each process deserves a new line
					print( "\n" );
		
					// remove orphaned "white" characters from command line
					shell_command_length = lib_string_trim( shell_command, shell_command_length );

					// terminate command from previous one
					shell_command[ shell_command_length ] = STD_ASCII_TERMINATOR;

					// empty command line?
					if( ! shell_command_length ) break;	// yep, show new prompt

					// try one of internal commands

					// directory change?
					if( lib_string_compare( shell_command, (uint8_t *) "cd", 2 ) ) {	// yes
						// remove "cd"
						shell_command_length = lib_string_word_remove( shell_command, shell_command_length, STD_ASCII_SPACE );

						// request change of root directory
						std_cd( (uint8_t *) shell_command );

						// new prompt
						break;
					}


					// clear entire screen and move cursor at beginning?
					if( lib_string_compare( shell_command, (uint8_t *) "clear", 5 ) ) {	// yes
						// send "clear" sequence
						print( "\e[2J" );

						// new prompt
						break;
					}

					// exit shell?
					if( lib_string_compare( shell_command, (uint8_t *) "exit", 4 ) )	// yes
						// nothing left to do
						return 0;

					// try to run program with given name and parameters
					int64_t shell_exec_pid = std_exec( shell_command, shell_command_length, EMPTY );

					// something went wrong?
					if( shell_exec_pid < 0 ) {
						// show information
						printf( "Command \e[38;5;196m%s\e[0m not found.", shell_command );

						// new prompt
						break;
					}

					// wait for its end if requested
					if( shell_command[ shell_command_length - 1 ] != '&' ) while( shell_exec_pid && std_pid_check( shell_exec_pid ) ) std_sleep( 1 );	// free up AP time until program execution ends

					// new prompt
					break;
				}

				// backspace key?
				if( keyboard -> key == STD_ASCII_BACKSPACE ) {
					// if command line is not empty
					if( ! shell_command_length ) continue;

					// remove previous character
					print( "\b" );

					// shorten command line content
					shell_command_length--;

					// retrieve next character from user
					continue;
				}

				// if key is not printable
				if( keyboard -> key < STD_ASCII_SPACE || keyboard -> key > STD_ASCII_TILDE ) continue;	// ignore

				// command line if full?
				if( shell_command_length == SHELL_COMMAND_limit ) continue;	// yes

				// store character in command line
				shell_command[ shell_command_length++ ] = (uint8_t) keyboard -> key;

				// send character to stream out
				print( (const char *) &keyboard -> key );
			} else
				// free up AP time
				std_sleep( 1 );	
		}
	}
}
