/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_sync( void ) {
	// requested flush?
	if( ! (wm_object_cache -> descriptor -> flags & STD_WINDOW_FLAG_flush) ) return;	// no

	// sync content of cache width graphics card
	uint32_t *cache_pixel = (uint32_t *) ((uintptr_t) wm_object_cache -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));
	for( uint64_t y = 0; y < framebuffer.height_pixel; y++ )
		for( uint64_t x = 0; x < framebuffer.width_pixel; x++ )
			framebuffer.base_address[ (y * (framebuffer.pitch_byte >> STD_VIDEO_DEPTH_shift)) + x ] = cache_pixel[ (y * framebuffer.width_pixel) + x ];

	// request parsed
	wm_object_cache -> descriptor -> flags ^= STD_WINDOW_FLAG_flush;
}
