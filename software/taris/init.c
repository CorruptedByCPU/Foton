/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void taris_init( void ) {
	// initialize interface library
	taris_interface.properties = (uint8_t *) &file_interface_start;
	lib_interface( (struct LIB_INTERFACE_STRUCTURE *) &taris_interface );

	// initialize RGL library
	taris_rgl = lib_rgl( TARIS_PLAYGROUND_WIDTH_pixel, TARIS_PLAYGROUND_HEIGHT_pixel, taris_interface.width, (uint32_t *) ((uintptr_t) taris_interface.descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ) + (((LIB_INTERFACE_HEADER_HEIGHT_pixel * taris_interface.width) + LIB_INTERFACE_BORDER_pixel) << STD_VIDEO_DEPTH_shift)) );

	// update window content on screen
	taris_interface.descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;
}