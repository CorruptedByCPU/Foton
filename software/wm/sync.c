/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_sync( void ) {
	// remove any overlapping
	wm_zone_substract();

	// for each object from bottom
	for( uint64_t j = wm -> list_limit; j > 0; j-- ) {
		// for every zone
		for( uint64_t i = 0; i < wm -> zone_limit; i++ ) {
			if( wm -> list[ j - 1 ] != wm -> zone[ i ].object ) continue;

			// copy the contents of the buffer to the memory space of the graphics card
			uint32_t *source = (uint32_t *) ((uintptr_t) wm -> canvas.descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ));
			for( uint64_t y = wm -> zone[ i ].y; y < wm -> zone[ i ].y + wm -> zone[ i ].height; y++ )
				for( uint64_t x = wm -> zone[ i ].x; x < wm -> zone[ i ].x + wm -> zone[ i ].width; x++ )
					wm -> framebuffer.base_address[ (y * (wm -> framebuffer.pitch_byte >> STD_VIDEO_DEPTH_shift)) + x ] = source[ (y * wm -> canvas.width) + x ] & ~STD_COLOR_mask;
		}
	}

	// all zones parsed
	wm -> zone_limit = EMPTY;
}
