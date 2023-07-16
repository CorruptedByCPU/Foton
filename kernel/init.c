/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

// definitions, that are always nice to have
#include	"stdint.h"
#include	"stddef.h"
#include	"stdbool.h"
#include	"stdarg.h"

// limine definitions
#include	"../limine/limine.h"

// limine requests
static volatile struct limine_framebuffer_request limine_framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0
};

// our mighty kernel
void init( void ) {
	// linear framebuffer is available (with 32 bits per pixel)?
	if( limine_framebuffer_request.response == NULL || limine_framebuffer_request.response -> framebuffer_count != 1 || limine_framebuffer_request.response -> framebuffers[ 0 ] -> bpp != 32 )
		// no, infinite loop (screen will be black)
		for(;;);

	// set pointer to first pixel of video memory area
	uint32_t *pixel = (uint32_t *) limine_framebuffer_request.response -> framebuffers[ 0 ] -> address;

	// number of pixels
	uint64_t count = limine_framebuffer_request.response -> framebuffers[ 0 ] -> width * limine_framebuffer_request.response -> framebuffers[ 0 ] -> height;

	// change all pixels color to RED (bright)
	while( count-- ) *(pixel++) = 0x00FF0000;

	// infinite loop
	for(;;);
}
