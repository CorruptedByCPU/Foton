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
void kinit( void ) {
	// linear framebuffer is available (with 32 bits per pixel)?
	if( limine_framebuffer_request.response == NULL || ! limine_framebuffer_request.response -> framebuffer_count || limine_framebuffer_request.response -> framebuffers[ 0 ] -> bpp != STD_VIDEO_DEPTH_bit )
		// no, infinite loop (screen will be black)
		while( TRUE );

	// update terminal properties
	kinit_terminal.width		= limine_framebuffer_request.response -> framebuffers[ 0 ] -> width;
	kinit_terminal.height		= limine_framebuffer_request.response -> framebuffers[ 0 ] -> height;
	kinit_terminal.base_address	= (uint32_t *) limine_framebuffer_request.response -> framebuffers[ 0 ] -> address;
	kinit_terminal.scanline_pixel	= limine_framebuffer_request.response -> framebuffers[ 0 ] -> pitch >> STD_VIDEO_DEPTH_shift;
	kinit_terminal.color_foreground	= STD_COLOR_WHITE;
	kinit_terminal.color_background	= STD_COLOR_BLACK_light;

	// initialize terminal
	lib_terminal( &kinit_terminal );

	// show welcome message
	const char welcome[] = "Foton, environment initialization.\n";
	lib_terminal_string( &kinit_terminal, welcome, sizeof( welcome ) );

	// infinite loop
	while( TRUE );
}
