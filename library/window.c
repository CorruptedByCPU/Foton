/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_WINDOW
		#include	"./window.h"
	#endif
	#ifndef	LIB_STRING
		#include	"./string.h"
	#endif

struct LIB_WINDOW_DESCRIPTOR *lib_window_event( struct LIB_WINDOW_DESCRIPTOR *descriptor ) {
	// acquired new descriptor properties?
	if( ! (descriptor -> flags & STD_WINDOW_FLAG_properties) ) return EMPTY;	// no

	// disable flag
	descriptor -> flags ^= STD_WINDOW_FLAG_properties;

	// minimal dimesions are preserved?
	if( descriptor -> width_limit > descriptor -> new_width ) descriptor -> new_width = descriptor -> width_limit;	// no, set correction
	if( descriptor -> height_limit > descriptor -> new_height ) descriptor -> new_height = descriptor -> height_limit;	// no, set correction

	// new descriptor properties
	struct LIB_WINDOW_DESCRIPTOR *window = lib_window( descriptor -> new_x, descriptor -> new_y, descriptor -> new_width, descriptor -> new_height );
	if( ! window ) return EMPTY;	// cannot create new descriptor

	// spread descriptor name
	window -> name_length = descriptor -> name_length;
	for( uint64_t i = 0; i < descriptor -> name_length; i++ ) window -> name[ i ] = descriptor -> name[ i ];

	// copy required descriptor properties from old one
	//----------------------------------------------------------------------
	window -> width_limit = descriptor -> width_limit;
	window -> height_limit = descriptor -> height_limit;
	//----------------------------------------------------------------------
	window -> flags = descriptor -> flags;
	//----------------------------------------------------------------------

	// release old descriptor
	descriptor -> flags |= STD_WINDOW_FLAG_release;

	// new descriptor created
	return window;
}

void lib_window_name( struct LIB_WINDOW_DESCRIPTOR *descriptor, uint8_t *name ) {
	// set window name
	descriptor -> name_length = lib_string_length( name );
	for( uint8_t i = 0; i < descriptor -> name_length; i++ ) descriptor -> name[ i ] = name[ i ];

	// inform Window Manager about new window name
	descriptor -> flags |= STD_WINDOW_FLAG_name;
}

struct LIB_WINDOW_DESCRIPTOR *lib_window( int16_t x, int16_t y, uint16_t width, uint16_t height ) {
	// obtain information about kernel framebuffer
	struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER framebuffer;
	std_framebuffer( (struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER *) &framebuffer );

	// allocate message container
	uint8_t data_request[ STD_IPC_SIZE_byte ] = { EMPTY };
	uint8_t data_answer[ STD_IPC_SIZE_byte ] = { EMPTY };

	// properties of messages
	struct STD_STRUCTURE_IPC_WINDOW_CREATE *request = (struct STD_STRUCTURE_IPC_WINDOW_CREATE *) &data_request;
	struct STD_STRUCTURE_IPC_WINDOW_DESCRIPTOR *answer = (struct STD_STRUCTURE_IPC_WINDOW_DESCRIPTOR *) &data_answer;

	//----------------------------------------------------------------------

	// window properties
	request -> ipc.type = STD_IPC_TYPE_default;
	request -> properties = STD_IPC_WINDOW_create;
	request -> x = x;
	request -> y = y;
	request -> width = width;
	request -> height = height;

	// center window?
	if( x == STD_MAX_unsigned && y == STD_MAX_unsigned ) { request -> x = (framebuffer.width_pixel >> STD_SHIFT_2) - (width >> STD_SHIFT_2); request -> y = (framebuffer.height_pixel >> STD_SHIFT_2) - (height >> STD_SHIFT_2); }

	// send request to Window Manager
	std_ipc_send( framebuffer.pid, (uint8_t *) request );

	// wait for answer
	uint64_t timeout = std_microtime() + 32768;
	while( (! std_ipc_receive( (uint8_t *) answer ) || answer -> ipc.type != STD_IPC_TYPE_default) && timeout > std_microtime() );

	// window assigned?
	if( ! answer -> descriptor ) return EMPTY;	// no

	// properties of console window
	return (struct LIB_WINDOW_DESCRIPTOR *) answer -> descriptor;
}
