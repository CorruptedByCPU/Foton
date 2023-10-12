/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/image.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions of Window Manager
	//----------------------------------------------------------------------
	#include	"./wm/config.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	// #include	"./gui/config.h"
	//----------------------------------------------------------------------
	// variables, routines, procedures
	//----------------------------------------------------------------------
	#include	"./gui/data.c"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	/* TEST TEST TEST TEST */

	// obtain information about kernel framebuffer
	struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER framebuffer;
	std_framebuffer( &framebuffer );

	// allocate container data
	uint8_t data[ STD_IPC_SIZE_byte ];

	// prepeare new window request
	struct WM_STRUCTURE_REQUEST *request = (struct WM_STRUCTURE_REQUEST *) &data;

	// wallpaper properties
	request -> x = EMPTY;
	request -> y = EMPTY;
	request -> width = framebuffer.width_pixel;
	request -> height = framebuffer.height_pixel;

	// send request
	std_ipc_send( framebuffer.owner_pid, (uint8_t *) request );

	// wait for answer
	while( ! std_ipc_receive( (uint8_t *) &data ) );

	// window created?
	struct WM_STRUCTURE_ANSWER *wallpaper = (struct WM_STRUCTURE_ANSWER *) &data;
	if( wallpaper -> descriptor ) {	// yes
		// properties of window
		struct WM_STRUCTURE_DESCRIPTOR *descriptor = (struct WM_STRUCTURE_DESCRIPTOR *) wallpaper -> descriptor;

		// fill window with default gradient
		uint32_t *pixel = (uint32_t *) ((uintptr_t) descriptor + sizeof( struct WM_STRUCTURE_DESCRIPTOR ));
		for( uint64_t y = 0; y < descriptor -> height; y++ )
			for( uint64_t x = 0; x < descriptor -> width; x++ )
				pixel[ (y * descriptor -> width) + x ] = 0xFF000000;

		// window content ready for display
		descriptor -> flags |= WM_OBJECT_FLAG_fixed_z | WM_OBJECT_FLAG_fixed_xy | WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_flush;
	}

	// taskbar properties
	request -> x = EMPTY;
	request -> y = framebuffer.height_pixel - 32;
	request -> width = framebuffer.width_pixel;
	request -> height = 32;

	// send request
	std_ipc_send( framebuffer.owner_pid, (uint8_t *) request );

	// wait for answer
	while( ! std_ipc_receive( (uint8_t *) &data ) );

	// window created?
	struct WM_STRUCTURE_ANSWER *taskbar = (struct WM_STRUCTURE_ANSWER *) &data;
	if( taskbar -> descriptor ) {	// yes
		// properties of window
		struct WM_STRUCTURE_DESCRIPTOR *descriptor = (struct WM_STRUCTURE_DESCRIPTOR *) taskbar -> descriptor;

		// fill window with default gradient
		uint32_t *pixel = (uint32_t *) ((uintptr_t) descriptor + sizeof( struct WM_STRUCTURE_DESCRIPTOR ));
		for( uint64_t y = 0; y < descriptor -> height; y++ )
			for( uint64_t x = 0; x < descriptor -> width; x++ )
				pixel[ (y * descriptor -> width) + x ] = 0xE0101010;

		// mark window as arbiter
		descriptor -> flags |= WM_OBJECT_FLAG_arbiter;

		// window content ready for display
		descriptor -> flags |= WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_flush;
	}

	// image properties
	struct LIB_IMAGE_TGA_STRUCTURE *source = (struct LIB_IMAGE_TGA_STRUCTURE *) &file_cursor_start;
	request -> x = framebuffer.width_pixel >> STD_SHIFT_2;
	request -> y = framebuffer.height_pixel >> STD_SHIFT_2;
	request -> width = source -> width;
	request -> height = source -> height;

	// send request
	std_ipc_send( framebuffer.owner_pid, (uint8_t *) request );

	// wait for answer
	while( ! std_ipc_receive( (uint8_t *) &data ) );

	// window created?
	struct WM_STRUCTURE_ANSWER *cursor = (struct WM_STRUCTURE_ANSWER *) &data;
	if( cursor -> descriptor ) {	// yes
		// properties of window
		struct WM_STRUCTURE_DESCRIPTOR *descriptor = (struct WM_STRUCTURE_DESCRIPTOR *) cursor -> descriptor;

		// load the image into the window space
		uint32_t *target = (uint32_t *) ((uintptr_t) descriptor + sizeof( struct WM_STRUCTURE_DESCRIPTOR ));
		lib_image_tga_parse( (uint8_t *) source, target, (uint64_t) file_cursor_end - (uint64_t) file_cursor_start );

		// mark object as cursor
		descriptor -> flags |= WM_OBJECT_FLAG_cursor;

		// window content ready for display
		descriptor -> flags |= WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_flush;
	}

	// hold the door
	while( TRUE );
}