/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/input.h"
	#include	"../library/path.h"
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
	// initialize input library
	struct LIB_INPUT_STRUCTURE *input = lib_input_init();

	// assign area for command prompt
	shell_command = (uint8_t *) malloc( SHELL_COMMAND_limit + 1 );

	// assign area for hostname
	hostname = (uint8_t *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( 63 + 1 ) >> STD_SHIFT_PAGE );

	// set header
	print( "\eXShell\e\\" );

	// new prompt loop
	while( TRUE ) {
		// retrieve stream meta data
		struct STD_STRUCTURE_STREAM_META stream_meta;
		while( ! std_stream_get( (uint8_t *) &stream_meta, STD_STREAM_OUT ) );

		// cursor at beginning of line?
		if( stream_meta.x ) print( "\n" );	// no, move cursor to next line

		// open current directory properties
		FILE *dir = fopen( (uint8_t *) ".", EMPTY );

		// open hostname file
		FILE *file = fopen( (uint8_t *) "system:/etc/hostname.txt", EMPTY );
		if( file ) {
			// load file content
			fread( file, hostname, 63 );

			// close file
			fclose( file );

			// remove "white" characters from first line
			lib_string_trim( hostname, lib_string_length( hostname ) );

			// limit to first word
			hostname[ lib_string_word_of_letters_and_digits( hostname, lib_string_length( hostname ) ) ] = STD_ASCII_TERMINATOR;
		}

		// properties of available storages
		uint64_t storage_id = std_storage_id();	// get current storage id
		struct STD_STRUCTURE_STORAGE *storage = (struct STD_STRUCTURE_STORAGE *) std_storage(); uint64_t entries = EMPTY;
		do { entries++; if( storage -> id == storage_id ) break; } while( (++storage) -> type );

		// show prompt
		printf( "\e[0m\e[P\e[38;5;47m%s \e[38;5;15m%s:%s \e[38;5;47m%%\e[0m ", hostname, storage -> name, (uint8_t *) &dir -> name );

		// close directory
		fclose( dir );

		// release storage properties
		while( (++storage) -> type ) { entries++; }; std_memory_release( (uintptr_t) storage, MACRO_PAGE_ALIGN_UP( sizeof( struct STD_STRUCTURE_STORAGE ) * (entries + 1) ) >> STD_SHIFT_PAGE );

		// receive command from user
		uint64_t shell_command_length = lib_input( input, shell_command, SHELL_COMMAND_limit, EMPTY, (uint8_t *) &shell_key_ctrl_left_semaphore );

		// empty command line?
		if( ! shell_command_length ) continue;	// yep, show new prompt

		// each process deserves a new line
		print( "\n" );

		// try one of internal commands

		// directory change?
		if( lib_string_compare( shell_command, (uint8_t *) "cd", 2 ) ) {	// yes
			// remove "cd"
			shell_command_length = lib_string_word_remove( shell_command, shell_command_length, STD_ASCII_SPACE );

			// request change of root directory
			if( ! std_cd( (uint8_t *) shell_command, shell_command_length ) )
				// error
				printf( "No such directory." );

			// new prompt
			continue;
		}


		// clear entire screen and move cursor at beginning?
		if( lib_string_compare( shell_command, (uint8_t *) "clear", 5 ) ) {	// yes
			// send "clear" sequence
			print( "\e[2J" );

			// new prompt
			continue;
		}

		// exit shell?
		if( lib_string_compare( shell_command, (uint8_t *) "exit", 4 ) )	// yes
			// nothing left to do
			return 0;

		// try to run program with given name and parameters
		uint8_t detach = FALSE;
		if( shell_command[ shell_command_length - 1 ] == '&' ) detach = TRUE;
		int64_t shell_exec_pid = std_exec( shell_command, shell_command_length, EMPTY, detach );

		// something went wrong?
		if( shell_exec_pid < 0 ) {
			// truncate command name
			shell_command[ lib_string_word( shell_command, shell_command_length ) ] = STD_ASCII_TERMINATOR;
			
			// show information
			printf( "Command \e[38;5;250m%s\e[0m not found.", shell_command );

			// new prompt
			continue;
		}

		// if requested
		if( ! detach ) {
			// wait for child end
			while( shell_exec_pid && std_pid_exist( shell_exec_pid ) ) {
				// pass all incomming messages to child
				uint8_t data[ STD_IPC_SIZE_byte ];
				while( std_ipc_receive( (uint8_t *) &data ) )
					// send this message forward to child
					std_ipc_send( shell_exec_pid, (uint8_t *) &data );
			
				// free up AP time until program execution ends
				sleep( TRUE );
			}

			// restore header
			print( "\eXShell\e\\" );
		}
	}
}
