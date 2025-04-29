/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void tiwyn_sync( void ) {
	// requested synchronization?
	if( ! (tiwyn_object_cache.descriptor -> flags & STD_WINDOW_FLAG_flush) ) return;	// no

	// copy the contents of the buffer to the memory space of the graphics card
	uint32_t *source = (uint32_t *) ((uintptr_t) tiwyn_object_cache.descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));
	for( uint64_t y = 0; y < tiwyn_framebuffer.height_pixel; y++ )
		for( uint64_t x = 0; x < tiwyn_framebuffer.width_pixel; x++ )
			tiwyn_framebuffer.base_address[ (y * (tiwyn_framebuffer.pitch_byte >> STD_VIDEO_DEPTH_shift)) + x ] = source[ (y * tiwyn_framebuffer.width_pixel) + x ];

	// request accepted
	tiwyn_object_cache.descriptor -> flags ^= STD_WINDOW_FLAG_flush;
}
