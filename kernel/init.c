/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// library
	//----------------------------------------------------------------------
	#include	"../library/color.h"
	#include	"../library/color.c"
	#include	"../library/font.h"
	#include	"../library/font.c"
	#include	"../library/terminal.h"
	#include	"../library/terminal.c"
	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#include	"../limine/limine.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"data.c"
	//----------------------------------------------------------------------

// our mighty init
void kernel_init( void ) {
	// linear framebuffer is available (with 32 bits per pixel)?
	if( limine_framebuffer_request.response == NULL || ! limine_framebuffer_request.response -> framebuffer_count || limine_framebuffer_request.response -> framebuffers[ 0 ] -> bpp != STD_VIDEO_DEPTH_bit )
		// no, infinite loop (screen will be black)
		while( TRUE );

	// update terminal properties
	kernel_terminal.width			= limine_framebuffer_request.response -> framebuffers[ 0 ] -> width;
	kernel_terminal.height			= limine_framebuffer_request.response -> framebuffers[ 0 ] -> height;
	kernel_terminal.base_address		= (uint32_t *) limine_framebuffer_request.response -> framebuffers[ 0 ] -> address;
	kernel_terminal.scanline_pixel		= limine_framebuffer_request.response -> framebuffers[ 0 ] -> pitch >> STD_VIDEO_DEPTH_shift;
	kernel_terminal.color_foreground	= STD_COLOR_WHITE;
	kernel_terminal.color_background	= STD_COLOR_BLACK_light;

	// initialize terminal
	lib_terminal( &kernel_terminal );

	// show welcome message
	const char welcome[] = "Foton, environment initialization.\n";
	lib_terminal_string( &kernel_terminal, welcome, sizeof( welcome ) );

	// infinite loop
	while( TRUE );
}
