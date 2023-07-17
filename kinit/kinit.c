/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	// static --------------------------------------------------------------
	#include	"../library/color.h"
	#include	"../library/color.c"
	#include	"../library/font.h"
	#include	"../library/font.c"
	//----------------------------------------------------------------------

// limine definitions
#include	"../limine/limine.h"

// limine requests
static volatile struct limine_framebuffer_request limine_framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0
};

// our mighty init
void kinit( void ) {
	// linear framebuffer is available (with 32 bits per pixel)?
	if( limine_framebuffer_request.response == NULL || limine_framebuffer_request.response -> framebuffer_count != 1 || limine_framebuffer_request.response -> framebuffers[ 0 ] -> bpp != 32 )
		// no, infinite loop (screen will be black)
		for(;;);

	// set pointer to first pixel of video memory area
	uint32_t *pixel = (uint32_t *) limine_framebuffer_request.response -> framebuffers[ 0 ] -> address;

	// change all pixels color to DARK
	for( uint64_t y = 0; y < limine_framebuffer_request.response -> framebuffers[ 0 ] -> height; y++ ) {
		for( uint64_t x = 0; x < limine_framebuffer_request.response -> framebuffers[ 0 ] -> width; x++ )
			pixel[ x ] = 0x00101010;

		// next line of pixels on framebuffer
		pixel = (uint32_t *) ((uint64_t) pixel + limine_framebuffer_request.response -> framebuffers[ 0 ] -> pitch);
	}

	// show welcome message
	const char welcome[ 34 ] = "Foton, environment initialization.";
	lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, welcome, sizeof( welcome ), STD_COLOR_GREEN_light, (uint32_t *) limine_framebuffer_request.response -> framebuffers[ 0 ] -> address, limine_framebuffer_request.response -> framebuffers[ 0 ] -> pitch >> 2, LIB_FONT_ALIGN_left );

	// infinite loop
	for(;;);
}
