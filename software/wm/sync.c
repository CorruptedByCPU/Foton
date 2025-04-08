/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_sync( void ) {
	// requested synchronization?
	if( ! (wm_object_cache.descriptor -> flags & STD_WINDOW_FLAG_flush) ) return;	// no

	// copy the contents of the buffer to the memory space of the graphics card
	uint32_t *source = (uint32_t *) ((uintptr_t) wm_object_cache.descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));
	for( uint64_t y = 0; y < kernel_framebuffer.height_pixel; y++ )
		for( uint64_t x = 0; x < kernel_framebuffer.width_pixel; x++ )
			kernel_framebuffer.base_address[ (y * (kernel_framebuffer.pitch_byte >> STD_VIDEO_DEPTH_shift)) + x ] = source[ (y * kernel_framebuffer.width_pixel) + x ];

	// request accepted
	wm_object_cache.descriptor -> flags ^= STD_WINDOW_FLAG_flush;
}
