/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_STRING
		#include	"../library/string.h"
	#endif

#define	SHELL_COMMAND_limit	4080

uint8_t *shell_command;

uint8_t shell_keyboard_status_alt_left = FALSE;
uint8_t	shell_keyboard_status_shift_left = FALSE;
uint8_t	shell_keyboard_status_ctrl_left = FALSE;

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// assign area for command prompt
	shell_command = (uint8_t *) malloc( SHELL_COMMAND_limit );

	// new prompt loop
	while( TRUE ) {
		// show prompt
		print( "\033[38;5;82m$\033[0m " );

		// length of string passed by user
		uint64_t shell_command_length = EMPTY;

		// readline
		while( TRUE ) {
			// incomming message
			uint8_t data[ STD_IPC_SIZE_byte ];
			if( std_ipc_receive( (uint8_t *) &data ) ) {
				// properties of incomming message
				struct STD_IPC_STRUCTURE_DEFAULT *ipc = (struct STD_IPC_STRUCTURE_DEFAULT *) &data;

				// message type: key?
				if( ipc -> type != STD_IPC_TYPE_keyboard ) continue;	// no

				// properties of Keyboard message
				struct STD_IPC_STRUCTURE_KEYBOARD *keyboard = (struct STD_IPC_STRUCTURE_KEYBOARD *) &data;

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
					// move cursor to new line
					print( "\n" );

					// remove orphaned "white" characters from command line
					shell_command_length = lib_string_trim( shell_command, shell_command_length );

					// try to run program with given name and parameters
					int64_t shell_exec_pid = std_exec( shell_command, shell_command_length, EMPTY );

					// wait for its end if requested
					if( shell_command[ shell_command_length - 1 ] != '&' ) while( shell_exec_pid && std_pid_check( shell_exec_pid ) );

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
				
				// or special key on hold?
				if( shell_keyboard_status_alt_left || shell_keyboard_status_ctrl_left ) continue;	// yes

				// store character in command line
				shell_command[ shell_command_length++ ] = (uint8_t) keyboard -> key;

				// send character to stream out
				print( (const char *) &keyboard -> key );
			}
		}
	}
}
