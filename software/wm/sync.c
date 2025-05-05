/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_sync( void ) {
	// copy the contents of the buffer to the memory space of the graphics card
	uint32_t *source = (uint32_t *) ((uintptr_t) wm -> canvas.descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));
	for( uint64_t y = 0; y < wm -> framebuffer.height_pixel; y++ )
		for( uint64_t x = 0; x < wm -> framebuffer.width_pixel; x++ )
			wm -> framebuffer.base_address[ (y * (wm -> framebuffer.pitch_byte >> STD_VIDEO_DEPTH_shift)) + x ] = source[ (y * wm -> framebuffer.width_pixel) + x ] & ~STD_COLOR_mask;
}
