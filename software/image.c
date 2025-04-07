/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/color.h"
	#include	"../library/image.h"
	#include	"../library/interface.h"
	#include	"../library/window.h"
	//----------------------------------------------------------------------
	// variables, routines, procedures
	//----------------------------------------------------------------------
	#include	"./image/config.h"
	#include	"./image/data.c"

uint8_t image_load( uint8_t *path ) {
	// properties of file
	FILE *file;

	// retrieve file information
	if( ! (file = fopen( path, EMPTY )) ) return FALSE;

	// properties of image
	struct LIB_IMAGE_STRUCTURE_TGA *image = (struct LIB_IMAGE_STRUCTURE_TGA *) malloc( file -> byte );

	// load file content
	fread( file, (uint8_t *) image, file -> byte );

	// convert image to RGBA
	image_source = (uint32_t *) malloc( (image -> width * image -> height) << STD_VIDEO_DEPTH_shift );
	lib_image_tga_parse( (uint8_t *) image, image_source, file -> byte );

	// no more needed
	fclose( file );

	// preserve image properties
	image_width = image -> width;
	image_height = image -> height;

	// release image content
	free( image );

	// done
	return TRUE;
}

void image_border( struct STD_STRUCTURE_WINDOW_DESCRIPTOR *image_window, uint32_t color, uint32_t color_shadow ) {
	// pointer of window content
	uint32_t *image_target = (uint32_t *) ((uintptr_t) image_window + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));

	// and point border
	for( uint16_t y = 0; y < image_window -> height; y++ )
		for( uint16_t x = 0; x < image_window -> width; x++ ) {
			if( ! x || ! y ) image_target[ (y * image_window -> width) + x ] = color;
			if( x == image_window -> width - 1 || y == image_window -> height - 1 ) image_target[ (y * image_window -> width) + x ] = color_shadow;
		}

}

void image_draw( struct STD_STRUCTURE_WINDOW_DESCRIPTOR *image_window ) {
	// pointer of window content
	uint32_t *image_target = (uint32_t *) ((uintptr_t) image_window + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));

	//----------------------------------------------------------------------

	// clean'up window area
	for( uint64_t i = 0; i < image_window -> width * image_window -> height; i++ ) image_target[ i ] = IMAGE_COLOR_BACKGROUND;

	//----------------------------------------------------------------------

	// image viewer area
	image_view_width = image_window -> width - (IMAGE_BORDER_pixel << STD_SHIFT_2);
	image_view_height = image_window -> height - (IMAGE_BORDER_pixel << STD_SHIFT_2);

	// keep aspect ratio of image
	double ratio = (double) image_width / (double) image_height;

	// calculate new image width/height
	uint64_t limit_width = image_view_width;
	uint64_t limit_height = (uint64_t) ((double) image_view_width / ratio);
	if( (uint64_t) ((double) image_view_width / ratio) > image_view_height ) { limit_width = (uint64_t) ((double) image_view_height * ratio); limit_height = image_view_height; }

	// center image inside window
	uint64_t newX = (image_view_width - limit_width) >> STD_SHIFT_2;
	uint64_t newY = (image_view_height - limit_height) >> STD_SHIFT_2;

	// pointer of image content
	image_target += (((newY + IMAGE_BORDER_pixel) * image_window -> width) + newX + IMAGE_BORDER_pixel);

	// copy scaled image content to workbench object
	for( uint16_t y = 0; y < limit_height; y++ ) {
		// calculate pixel location inside image
		uint64_t ny = y * image_height / limit_height;

		for( uint16_t x = 0; x < limit_width; x++ ) {
			// calculate pixel location inside image
			uint64_t nx = x * image_width / limit_width;

			// copy
			image_target[ (y * image_window -> width) + x ] = lib_color_blend( image_target[ (y * image_window -> width) + x ], image_source[ (ny * image_width) + nx ] );
		}
	}
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// nothing to do?
	if( argc < 2 ) return 0;	// yes

	// load image content
	if( ! image_load( argv[ 1 ] ) ) return 0;	// cannot load image

	//----------------------------------------------------------------------

	// obtain information about kernel framebuffer
	struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER framebuffer;
	std_framebuffer( (struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER *) &framebuffer );

	// image dimension larger than 80% of framebuffer?
	image_view_width = image_width + (IMAGE_BORDER_pixel << STD_SHIFT_2); if( image_width > (uint64_t) ((double) framebuffer.width_pixel * (double) 0.8f) ) image_view_width = (uint64_t) ((double) framebuffer.width_pixel * (double) 0.8f);
	image_view_height = image_height + (IMAGE_BORDER_pixel << STD_SHIFT_2); if( image_height > (uint64_t) ((double) framebuffer.height_pixel * (double) 0.8f) ) image_view_height = (uint64_t) ((double) framebuffer.height_pixel * (double) 0.8f);

	// create window
	image_window = (struct STD_STRUCTURE_WINDOW_DESCRIPTOR *) lib_window( -1, -1, image_view_width, image_view_height );
	if( ! image_window ) return EMPTY;	// close

	// set window name as opened file
	image_window -> name_length = lib_string_length( lib_string_basename( argv[ 1 ] ) );
	for( uint8_t i = 0; i < image_window -> name_length; i++ ) image_window -> name[ i ] = lib_string_basename( argv[ 1 ] )[ i ];

	// set minimal window size
	image_window -> width_limit = LIB_INTERFACE_HEADER_HEIGHT_pixel;
	image_window -> height_limit = LIB_INTERFACE_HEADER_HEIGHT_pixel;

	//----------------------------------------------------------------------

	// insert image into window
	image_draw( image_window );

	//----------------------------------------------------------------------

	// update window content on screen
	image_window -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_name | STD_WINDOW_FLAG_resizable | STD_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// main loop
	while( TRUE ) {
		// free up AP time
		sleep( TRUE );

		// window state
		if( (image_window -> flags & STD_WINDOW_FLAG_active) != image_window_semaphore ) {
			if( image_window -> flags & STD_WINDOW_FLAG_active ) image_border( image_window, IMAGE_BORDER_COLOR_default, IMAGE_BORDER_COLOR_default_shadow );
			else image_border( image_window, IMAGE_BORDER_COLOR_inactive_shadow, IMAGE_BORDER_COLOR_inactive_shadow );

			// preserve current state
			image_window_semaphore = image_window -> flags & STD_WINDOW_FLAG_active;

			// update window content on screen
			image_window -> flags |= STD_WINDOW_FLAG_flush;
		}

		// check events from interface
		struct STD_STRUCTURE_WINDOW_DESCRIPTOR *new = (struct STD_STRUCTURE_WINDOW_DESCRIPTOR *) lib_window_event( image_window );
		if( new ) {
			// update window pointer
			image_window = new;

			// redraw image
			image_draw( image_window );

			// update window content on screen
			image_window -> flags |= STD_WINDOW_FLAG_flush;
		}

		// incomming message
		uint8_t ipc_keyboard[ STD_IPC_SIZE_byte ] = { EMPTY };
		if( std_ipc_receive_by_type( (uint8_t *) &ipc_keyboard, STD_IPC_TYPE_keyboard ) ) {
			// message properties
			struct STD_STRUCTURE_IPC_KEYBOARD *keyboard = (struct STD_STRUCTURE_IPC_KEYBOARD *) &ipc_keyboard;

			// exit?
			if( keyboard -> key == STD_KEY_ESC ) break;	// yes
		}
	}

	// exit
	return 0;
}
