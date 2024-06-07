/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/integer.h"
	#include	"../library/string.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./tm/data.c"
	//----------------------------------------------------------------------
	// routines, procedures
	//----------------------------------------------------------------------
	#include	"./tm/format.c"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// set window header, clear and disable cursor
	print( "\eXTask Manager\e\\\e[N" );

	// main loop
	while( TRUE ) {
		// old stream meta
		struct STD_STREAM_STRUCTURE_META meta = top_stream_meta;

		// retrieve stream meta data
		std_stream_get( (uint8_t *) &top_stream_meta, STD_STREAM_OUT );

		// show interface
		printf( "\e[0m\e[2J%s", top_string_interface );

		// entry number
		uint64_t entry = 0;
		uint64_t entry_visible = 0;
		uint64_t entry_selected = 0;

		// set cursor at first entry
		print( "\e[0;1H" );

		// retrieve list of running tasks
		struct STD_SYSCALL_STRUCTURE_TASK *task = (struct STD_SYSCALL_STRUCTURE_TASK *) std_task();

		// show each task
		do {
			// leave entry?
			if( task[ entry ].flags & STD_TASK_FLAG_module && top_hide_modules ) continue;	// yes

			// mark selected entry
			if( entry_visible == top_line_selected ) {
				// mark entry
				entry_selected = entry;

				// change colors
				print( "\e[38;5;0m\e[48;5;42m\e[2K" );
			} else {
				// default coloristics
				print( "\e[0m" );

				// mark thread entry
				if( task[ entry ].flags & STD_TASK_FLAG_thread ) print( "\e[48;5;233m\e[38;5;250m" );
			
				// mark module entry
				if( task[ entry ].flags & STD_TASK_FLAG_module ) print( "\e[38;5;1m" );
			}

			// show properties of entry

			// process ID
			printf( "\e[2K%4u ", task[ entry ].pid );

			// memory use
			top_size( (task[ entry ].page + task[ entry ].stack) << STD_SHIFT_PAGE );

			// show process name
			uint64_t name_length = lib_string_word( task[ entry ].name, task[ entry ].name_length ); task[ entry ].name[ name_length ] = STD_ASCII_TERMINATOR;
			printf( " %s", task[ entry ].name );

			// and parameters (if exist)
			uint8_t *argv = (uint8_t *) &task[ entry ].name[ name_length + 1 ];
			uint64_t argv_length = lib_string_trim( argv, task[ entry ].name_length - name_length );
			if( argv_length ) printf( " \e[38;5;240m%s", argv );

			// next line
			print( "\n" );

			// entry showed
			entry_visible++;
		// until end of list
		} while( task[ ++entry ].flags );

		// wait before update
		while( std_uptime() < top_update_next ) {
			// recieve key
			uint16_t key = getkey(); if( ! key ) continue;	// nothing

			// exit Task Manager?
			if( key == 'q' ) return 0;	// yes

			// arrow down?
			if( key == STD_KEY_ARROW_DOWN && top_line_selected < entry_visible - 1 ) { top_line_selected++; break; }

			// arrow up?
			if( key == STD_KEY_ARROW_UP && top_line_selected ) { top_line_selected--; break; }

			// kill selected process?
			if( key == 'k' ) { std_kill( task[ entry_selected ].pid ); if( top_line_selected ) top_line_selected--; break; }

			// release rest of CPU time
			std_sleep( TRUE );
		}

		// release list
		std_memory_release( (uintptr_t) task, MACRO_PAGE_ALIGN_UP( sizeof( struct STD_SYSCALL_STRUCTURE_TASK ) * ++entry ) >> STD_SHIFT_PAGE );

		// set next update
		top_update_next = std_uptime() + top_update_limit;
	}

	// end of Task Manager
	return 0;
}