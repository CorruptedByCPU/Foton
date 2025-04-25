/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_sync( void ) {
	// requested synchronization?
	if( ! (wm_object_cache.descriptor -> flags & STD_WINDOW_FLAG_flush) ) return;	// no

	// copy the contents of the buffer to the memory space of the graphics card
	uint32_t *source = (uint32_t *) ((uintptr_t) wm_object_cache.descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));
	for( uint64_t y = 0; y < wm_framebuffer.height_pixel; y++ )
		for( uint64_t x = 0; x < wm_framebuffer.width_pixel; x++ )
			wm_framebuffer.base_address[ (y * (wm_framebuffer.pitch_byte >> STD_VIDEO_DEPTH_shift)) + x ] = source[ (y * wm_framebuffer.width_pixel) + x ];

	// request accepted
	wm_object_cache.descriptor -> flags ^= STD_WINDOW_FLAG_flush;
}
