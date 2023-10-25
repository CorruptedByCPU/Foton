/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_sync( void ) {
	// requested synchronization?
	if( ! (wm_object_cache.descriptor -> flags & STD_WINDOW_FLAG_flush) ) return;	// no

	// copy the contents of the buffer to the memory space of the graphics card
	uint32_t *source = (uint32_t *) ((uintptr_t) wm_object_cache.descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ));
	for( uint32_t i = 0; i < (framebuffer.width_pixel * framebuffer.height_pixel); i++ )
			framebuffer.base_address[ i ] = source[ i ];

	// request accepted
	wm_object_cache.descriptor -> flags ^= STD_WINDOW_FLAG_flush;
}