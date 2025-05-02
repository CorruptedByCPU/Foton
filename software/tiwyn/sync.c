/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void tiwyn_sync( void ) {
	// requested synchronization?
	if( ! (tiwyn -> canvas.descriptor -> flags & LIB_WINDOW_FLAG_flush) ) return;	// no

	// copy the contents of the buffer to the memory space of the graphics card
	uint32_t *source = (uint32_t *) ((uintptr_t) tiwyn -> canvas.descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));
	for( uint64_t y = 0; y < tiwyn -> framebuffer.height_pixel; y++ )
		for( uint64_t x = 0; x < tiwyn -> framebuffer.width_pixel; x++ )
			tiwyn -> framebuffer.base_address[ (y * (tiwyn -> framebuffer.pitch_byte >> STD_VIDEO_DEPTH_shift)) + x ] = source[ (y * tiwyn -> framebuffer.width_pixel) + x ];

	// request accepted
	tiwyn -> canvas.descriptor -> flags ^= LIB_WINDOW_FLAG_flush;
}
