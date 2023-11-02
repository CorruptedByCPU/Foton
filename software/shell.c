/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
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

				// if end of line
				if( keyboard -> key == STD_ASCII_RETURN ) {
					// move cursor to new line
					print( "\n" );

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

				// new key for command
				shell_command_length++;

				// debug
				print( (const char *) &keyboard -> key );
			}
		}
	}
}
