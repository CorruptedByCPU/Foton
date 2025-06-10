/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_sync( void ) {
	// properties of zone list
	struct WM_STRUCTURE_ZONE *zone = wm -> zone;

	// properties of object list
	struct WM_STRUCTURE_OBJECT **list = wm -> list;

	// for each object from bottom
	for( uint64_t j = wm -> list_limit; j > 0; j-- ) {
		// for every zone
		for( uint64_t i = 0; i < wm -> zone_limit; i++ ) {
			if( list[ j - 1 ] != zone[ i ].object ) continue;

			// copy the contents of the buffer to the memory space of the graphics card
			uint32_t *source = (uint32_t *) ((uintptr_t) wm -> canvas.descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ));
			for( uint64_t y = zone[ i ].y; y < zone[ i ].y + zone[ i ].height; y++ )
				for( uint64_t x = zone[ i ].x; x < zone[ i ].x + zone[ i ].width; x++ )
					wm -> framebuffer.base_address[ (y * (wm -> framebuffer.pitch_byte >> STD_VIDEO_DEPTH_shift)) + x ] = source[ (y * wm -> canvas.width) + x ] & ~STD_COLOR_mask;
		}
	}

	// all zones parsed
	wm -> zone_limit = EMPTY;
}
