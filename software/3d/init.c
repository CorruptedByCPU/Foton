/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t init( void ) {
	// obtain information about kernels framebuffer
	std_framebuffer( &framebuffer );

	// unsupported pitch size?
	if( framebuffer.pitch_byte >> STD_VIDEO_DEPTH_shift > framebuffer.width_pixel ) return TRUE;	// yes

	// initialize RGL library
	rgl = lib_rgl( framebuffer.width_pixel, framebuffer.height_pixel - LIB_FONT_HEIGHT_pixel, framebuffer.base_address + (framebuffer.width_pixel * LIB_FONT_HEIGHT_pixel) );

	//----------------------------------------------------------------------

	object_load();

	// execute interface as separate thread
	std_thread( (uintptr_t) &interface, (uint8_t *) "demo-interface", 14 );

	// it's ok
	return FALSE;
}