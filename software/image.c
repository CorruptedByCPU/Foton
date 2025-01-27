/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/image.h"
	#include	"../library/interface.h"
	//----------------------------------------------------------------------
	// variables, routines, procedures
	//----------------------------------------------------------------------
	#include	"./image/config.h"
	#include	"./image/data.c"

void image_close( void ) {
	// end of program
	exit();
}

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
	image_pixel = (uint32_t *) malloc( (image -> width * image -> height) << STD_VIDEO_DEPTH_shift );
	lib_image_tga_parse( (uint8_t *) image, image_pixel, file -> byte );

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

void image_draw( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// image viewer area
	uint64_t view_width = interface -> width - (LIB_INTERFACE_BORDER_pixel + LIB_INTERFACE_BORDER_pixel);
	uint64_t view_height = interface -> height - (LIB_INTERFACE_HEADER_HEIGHT_pixel + LIB_INTERFACE_BORDER_pixel);

	// keep aspect ratio of image
	double ratio = (double) image_width / (double) image_height;

	// calculate new image width/height
	uint64_t limit_width = view_width;
	uint64_t limit_height = (uint64_t) ((double) view_width / ratio);
	if( (uint64_t) ((double) view_width / ratio) > view_height ) { limit_width = (uint64_t) ((double) view_height * ratio); limit_height = view_height; }

	// center image inside window
	uint64_t newX = (view_width - limit_width) >> STD_SHIFT_2;
	uint64_t newY = (view_height - limit_height) >> STD_SHIFT_2;

	// properties of workbench area content
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ) + ((((LIB_INTERFACE_HEADER_HEIGHT_pixel + newY) * interface -> width) + newX + LIB_INTERFACE_BORDER_pixel) << STD_VIDEO_DEPTH_shift));

	// copy scaled image content to workbench object
	for( uint16_t y = 0; y < limit_height; y++ )
		for( uint16_t x = 0; x < limit_width; x++ ) {
			// calculate pixel location inside image
			uint64_t nx = x * image_width / limit_width;
			uint64_t ny = y * image_height / limit_height;

			// copy
			pixel[ (y * interface -> width) + x ] = image_pixel[ (ny * image_width) + nx ];
		}
}

// uint8_t image_window( void ) {
// 	// obtain information about kernel framebuffer
// 	struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER tmp_framebuffer;
// 	std_framebuffer( (struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER *) &tmp_framebuffer );

// 	// allocate gui data container
// 	uint8_t tmp_request[ STD_IPC_SIZE_byte ] = { EMPTY };
// 	// allocate gui data container
// 	uint8_t tmp_answer[ STD_IPC_SIZE_byte ] = { EMPTY };

// 	// prepeare new window request
// 	struct STD_STRUCTURE_IPC_WINDOW *request = (struct STD_STRUCTURE_IPC_WINDOW *) &tmp_request;
// 	struct STD_STRUCTURE_IPC_WINDOW_DESCRIPTOR *answer = (struct STD_STRUCTURE_IPC_WINDOW_DESCRIPTOR *) &tmp_answer;

// 	//----------------------------------------------------------------------

// 	// default values
// 	image_window_width = image_width;
// 	image_window_height = image_height;

// 	// window properties
// 	request -> ipc.type = STD_IPC_TYPE_event;
// 	if( image_width < (tmp_framebuffer.width_pixel >> STD_SHIFT_2) ) request -> width = image_width;
// 	else { image_window_width = tmp_framebuffer.width_pixel >> STD_SHIFT_2; request -> width = image_window_width; }
// 	if( image_height < (tmp_framebuffer.height_pixel >> STD_SHIFT_2) ) request -> height = image_height;
// 	else { image_window_height = tmp_framebuffer.height_pixel >> STD_SHIFT_2; request -> height = image_window_height; }

// 	// always center the window
// 	request -> x = (tmp_framebuffer.width_pixel >> STD_SHIFT_2) - (image_window_width >> STD_SHIFT_2);
// 	request -> y = (tmp_framebuffer.height_pixel >> STD_SHIFT_2) - (image_window_height >> STD_SHIFT_2);

// 	// send request to Window Manager
// 	std_ipc_send( tmp_framebuffer.pid, (uint8_t *) request );

// 	// wait for answer
// 	uint64_t tmp_timeout = std_microtime() + 32768;	// TODO, HPET, RTC...
// 	while( (! std_ipc_receive( (uint8_t *) answer ) || answer -> ipc.type != STD_IPC_TYPE_event) && tmp_timeout > std_microtime() );

// 	// window assigned?
// 	if( ! answer -> descriptor ) return FALSE;	// nope

// 	// preserve descriptor pointer
// 	image_descriptor = (struct STD_STRUCTURE_WINDOW_DESCRIPTOR *) answer -> descriptor;

// 	// done
// 	return TRUE;
// }

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// nothing to do?
	if( argc < 2 ) return 0;	// yes

	// load image content
	if( ! image_load( argv[ 1 ] ) ) return 0;	// cannot load image

	//----------------------------------------------------------------------

	// alloc area for interface properties
	struct LIB_INTERFACE_STRUCTURE *image_interface = (struct LIB_INTERFACE_STRUCTURE *) malloc( sizeof( struct LIB_INTERFACE_STRUCTURE ) );

	// initialize interface library
	image_interface -> properties = (uint8_t *) &file_interface_start;
	if( ! lib_interface( image_interface ) ) { log( "Cannot create window.\n" ); exit(); }

	// set minimal window size as current (ascpet ratio: 16:9)
	image_interface -> min_width = 300;
	image_interface -> min_height = 168;

	//----------------------------------------------------------------------

	// find entry of ID: 0
	struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *control = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) lib_interface_element_by_id( image_interface, 0 );
	control -> event = (void *) image_close;	// assign executable function to element

	//----------------------------------------------------------------------

	// insert image into window
	image_draw( image_interface );

	//----------------------------------------------------------------------

	// update window content on screen
	image_interface -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_resizable | STD_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// main loop
	while( TRUE ) {
		// free up AP time
		sleep( TRUE );

		// check events from interface
		struct LIB_INTERFACE_STRUCTURE *new = EMPTY;
		if( (new = lib_interface_event( image_interface )) ) {
			// update interface pointer
			image_interface = new;

			// insert image into window
			image_draw( image_interface );

			// update window content on screen
			image_interface -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
		}

		// check events from keyboard
		uint16_t key = lib_interface_event_keyboard( image_interface );

		// exit?
		if( key == STD_ASCII_ESC ) break;	// yes
	}

	// exit
	return 0;
}
