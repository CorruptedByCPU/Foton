/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_JSON
		#include	"./json.h"
	#endif
	#ifndef	LIB_INTERFACE
		#include	"./interface.h"
	#endif

uint8_t lib_interface_string_window[] = "window";
uint8_t lib_interface_string_width[] = "width";
uint8_t lib_interface_string_height[] = "height";
uint8_t lib_interface_string_value[] = "value";

void lib_interface( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// prepare JSON structure for parsing
	lib_json_squeeze( interface -> properties );

	// // convert interface properties to a more accessible format
	lib_interface_convert( interface );

	// if dimensions aquired from JSON structure
	if( interface -> width && interface -> height ) {
		// create window
		lib_interface_window( interface );
	}
}

void lib_interface_convert( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// properties of new interface format
	// uint8_t *properties = (uint8_t *) malloc( TRUE );
	// uint64_t i = EMPTY;	// properties index

	// properties of JSON structure
	struct LIB_JSON_STRUCTURE json = lib_json( interface -> properties );

	// parse elements inside JSON structure
	do {
		// window object?
		if( lib_json_key( json, (uint8_t *) &lib_interface_string_window ) ) {
			// window object properties
			struct LIB_JSON_STRUCTURE json_window = lib_json( (uint8_t *) json.value );

			// parse all keys of this object
			do {
				// retrieve width value
				if( lib_json_key( json_window, (uint8_t *) &lib_interface_string_width ) ) interface -> width = json_window.value;
	
				// retrieve height value
				if( lib_json_key( json_window, (uint8_t *) &lib_interface_string_height ) ) interface -> height = json_window.value;
			// next key
			} while( lib_json_next( (struct LIB_JSON_STRUCTURE *) &json_window ) );

		}
	// until no more elements
	} while( lib_json_next( (struct LIB_JSON_STRUCTURE *) &json ) );

	// store new properties pointer
	// interface -> properties = properties;
};

void lib_interface_window( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// obtain information about kernel framebuffer
	struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER kernel_framebuffer;
	std_framebuffer( &kernel_framebuffer );

	// remember Graphical User Interface PID
	int64_t gui_pid = kernel_framebuffer.pid;

	// allocate gui data container
	uint8_t gui_data[ STD_IPC_SIZE_byte ];

	// prepeare new window request
	struct STD_WINDOW_STRUCTURE_REQUEST *gui_request = (struct STD_WINDOW_STRUCTURE_REQUEST *) &gui_data;
	struct STD_WINDOW_STRUCTURE_ANSWER *gui_answer = EMPTY;	// answer will be in here

	//----------------------------------------------------------------------

	// wallpaper window properties
	gui_request -> width = interface -> width;
	gui_request -> height = interface -> height;

	// send request to Graphical User Interface
	std_ipc_send( gui_pid, (uint8_t *) gui_request );

	// wait for answer
	while( ! std_ipc_receive( (uint8_t *) gui_data ) );

	// window assigned?
	gui_answer = (struct STD_WINDOW_STRUCTURE_ANSWER *) &gui_data;
	if( ! gui_answer -> descriptor ) return;	// no

	// properties of console window
	interface -> descriptor = (struct STD_WINDOW_STRUCTURE_DESCRIPTOR *) gui_answer -> descriptor;

	// fill window with default background
	uint32_t *window_pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ));
	for( uint16_t y = 0; y < interface -> height; y++ )
		for( uint16_t x = 0; x < interface -> width; x++ )
			// draw pixel
			window_pixel[ (y * interface -> width) + x ] = STD_COLOR_BLACK;
}