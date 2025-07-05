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

struct LIB_WINDOW_STRUCTURE *lib_window_event( struct LIB_WINDOW_STRUCTURE *current ) {
	// acquired new descriptor properties?
	if( ! (current -> flags & LIB_WINDOW_FLAG_properties) ) return EMPTY;	// no

	// disable flag
	current -> flags ^= LIB_WINDOW_FLAG_properties;

	// minimal dimesions are preserved?
	if( current -> width_minimal > current -> new_width ) current -> new_width = current -> width_minimal;	// no, set correction
	if( current -> height_minimal > current -> new_height ) current -> new_height = current -> height_minimal;	// no, set correction

	// new descriptor properties
	struct LIB_WINDOW_STRUCTURE *new = lib_window( current -> new_x, current -> new_y, current -> new_width, current -> new_height );
	if( ! new ) return EMPTY;	// cannot create new descriptor

	// spread descriptor name
	for( uint64_t i = 0; i < current -> name_length; i++ ) new -> name[ new -> name_length++ ] = current -> name[ i ];

	// set new descriptor properties
	//----------------------------------------------------------------------
	// new -> current_x	= current -> new_x;
	// new -> current_y	= current -> new_y;
	// new -> current_width	= current -> new_width;
	// new -> current_height	= current -> new_height;
	//----------------------------------------------------------------------
	// copy required descriptor properties from old one
	//----------------------------------------------------------------------
	new -> width_minimal	= current -> width_minimal;
	new -> height_minimal	= current -> height_minimal;
	//----------------------------------------------------------------------
	new -> old_x		= current -> old_x;
	new -> old_y		= current -> old_y;
	new -> old_width	= current -> old_width;
	new -> old_height	= current -> old_height;
	//----------------------------------------------------------------------
	new -> flags		= current -> flags & ~LIB_WINDOW_FLAG_visible;
	//----------------------------------------------------------------------

	// release old descriptor
	current -> flags |= LIB_WINDOW_FLAG_release;

	// new descriptor created
	return new;
}

void lib_window_name( struct LIB_WINDOW_STRUCTURE *descriptor, uint8_t *name ) {
	// set window name
	descriptor -> name_length = lib_string_length( name );
	for( uint8_t i = 0; i < descriptor -> name_length; i++ ) descriptor -> name[ i ] = name[ i ];

	// inform Window Manager about new window name
	descriptor -> flags |= LIB_WINDOW_FLAG_name;
}

struct LIB_WINDOW_STRUCTURE *lib_window( int16_t x, int16_t y, uint16_t width, uint16_t height ) {
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
	request -> ipc.type	= STD_IPC_TYPE_default;
	request -> properties	= STD_IPC_WINDOW_create;
	request -> x		= x;
	request -> y		= y;
	request -> width	= width;
	request -> height	= height;

	// center window?
	if( x == STD_MAX_unsigned ) request -> x = (framebuffer.width_pixel >> STD_SHIFT_2) - (width >> STD_SHIFT_2);
	if( y == STD_MAX_unsigned ) request -> y = (framebuffer.height_pixel >> STD_SHIFT_2) - (height >> STD_SHIFT_2);

	// send request to Window Manager
	std_ipc_send( framebuffer.pid, (uint8_t *) request );

	// wait for answer
	uint64_t timeout = std_microtime() + 32768;
	while( (! std_ipc_receive( (uint8_t *) answer ) || answer -> ipc.type != STD_IPC_TYPE_default) && timeout > std_microtime() );

	// window assigned?
	if( ! answer -> descriptor ) return EMPTY;	// no

	// set default properties
	struct LIB_WINDOW_STRUCTURE *window = (struct LIB_WINDOW_STRUCTURE *) answer -> descriptor;

	// properties of console window
	return (struct LIB_WINDOW_STRUCTURE *) answer -> descriptor;
}
