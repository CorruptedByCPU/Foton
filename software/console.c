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
	#include	"./wm/config.h"
	//----------------------------------------------------------------------
	// variables, routines, procedures
	//----------------------------------------------------------------------
	#include	"./console/config.h"
	#include	"./console/data.c"
	#include	"./console/vt100.c"
	#include	"./console/init.c"

void close( void ) {
	// end of program
	exit();
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize console window
	console_init();

	// file selected?
	if( argc > 1 )	{
		// prepare area for path
		uint64_t file_path_length = 0;
		uint8_t *file_path = malloc( lib_integer_limit_unsigned( MACRO_SIZEOF( struct STD_FILE_STRUCTURE, name_length ) ) );

		// compose file name with args
		for( uint64_t i = 1; i < argc; i++ ) {
			// set file properties
			for( uint8_t j = 0; j < lib_string_length( argv[ i ] ); j++ ) file_path[ file_path_length++ ] = argv[ i ][ j ];

			// add separator of path
			file_path[ file_path_length++ ] = STD_ASCII_SPACE;
		}

		// open in console selected program
		console_pid_of_shell = std_exec( file_path, file_path_length, STD_STREAM_FLOW_out_to_parent_in );

		// release path
		// free( file_path );
	} else
		// run Shell program
		console_pid_of_shell = std_exec( (uint8_t *) "shell", 5, STD_STREAM_FLOW_out_to_parent_in );

	// main look
	while( TRUE ) {
		// end of shell?
		if( ! std_pid_check( console_pid_of_shell ) ) return 0;	// quit from console too

		// check events from interface
		if( ! console_the_master_of_puppets ) {
			// check incomming events
			struct LIB_INTERFACE_STRUCTURE *new = EMPTY;
			if( (new = lib_interface_event( console_interface )) ) {
				// update interface pointer
				console_interface = new;

				// initialize new terminal library
				console_terminal -> width		= console_interface -> width - ((LIB_INTERFACE_BORDER_pixel + console_interface -> descriptor -> offset) << STD_SHIFT_2);
				console_terminal -> height		= console_interface -> height - (LIB_INTERFACE_HEADER_HEIGHT_pixel + 1 + (console_interface -> descriptor -> offset << STD_SHIFT_2));
				console_terminal -> base_address	= (uint32_t *) ((uintptr_t) console_interface -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR )) + ((LIB_INTERFACE_HEADER_HEIGHT_pixel + console_interface -> descriptor -> offset) * console_interface -> width) + LIB_INTERFACE_BORDER_pixel;
				console_terminal -> scanline_pixel	= console_interface -> width;
				lib_terminal_reload( console_terminal );

				// default meta properties of stream
				console_stream_meta.width = console_terminal -> width_char;
				console_stream_meta.height = console_terminal -> height_char;

				// set default meta data of input stream
				std_stream_set( (uint8_t *) &console_stream_meta, STD_STREAM_IN );

				// update window content on screen
				console_interface -> descriptor -> flags |= STD_WINDOW_FLAG_resizable | STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;
			}
		}

		// are we the captain?
		if( console_the_master_of_puppets ) {
			// check keyboard cache
			uint16_t key = std_keyboard();

			// incomming key?
			if( key ) {
				// properties of keyboard message
				uint8_t data[ STD_IPC_SIZE_byte ];
				struct STD_IPC_STRUCTURE_KEYBOARD *keyboard = (struct STD_IPC_STRUCTURE_KEYBOARD *) &data;

				// IPC type
				keyboard -> ipc.type = STD_IPC_TYPE_keyboard;
				keyboard -> key = key;	// and key code

				// send this message to child
				std_ipc_send( console_pid_of_shell, (uint8_t *) &data );
			}
		} else {
			// incomming message
			uint8_t data[ STD_IPC_SIZE_byte ];
			if( std_ipc_receive( (uint8_t *) &data ) )
				// send this message forward to child
				std_ipc_send( console_pid_of_shell, (uint8_t *) &data );
		}

		// get data from input stream
		uint64_t console_stream_length = std_stream_in( console_stream_in );

		// if there is incomming stream
		if( console_stream_length ) {
			// disable cursor, no CPU power waste
			lib_terminal_cursor_disable( console_terminal );

			// parse all characters from stream
			for( uint32_t i = 0; i < console_stream_length; ) {
				// sequence length
				uint8_t sequence_length = console_vt100( (uint8_t *) &console_stream_in[ i ], console_stream_length - i );

				// display character
				if( ! sequence_length ) lib_terminal_char( console_terminal, console_stream_in[ i++ ] );

				// move forward of sequence
				i += sequence_length;
			}

			// turn on the cursor
			lib_terminal_cursor_enable( console_terminal );

			// are we the captain?
			if( console_the_master_of_puppets )
				// update kernels framebuffer
				for( uint64_t y = 0; y < console_terminal -> height; y++ )
					for( uint64_t x = 0; x < console_terminal -> width; x++ )
						kernel_framebuffer.base_address[ (y * kernel_framebuffer.width_pixel) + x ] = console_terminal -> base_address[ (y * console_terminal -> scanline_pixel) + x ];
			else
				// update window content on screen
				console_interface -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

			// set new meta properties of stream
			console_stream_meta.x = console_terminal -> cursor_x;
			console_stream_meta.y = console_terminal -> cursor_y;

			// update metadata of input stream
			std_stream_set( (uint8_t *) &console_stream_meta, STD_STREAM_IN );
		} else
			// free up AP time
			std_sleep( TRUE );
	}

	// hold the door
	while( TRUE );
}