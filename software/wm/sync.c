/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_sync( void ) {
	// properties of zone list
	struct WM_STRUCTURE_ZONE *zone = wm -> zone;

	// for every zone
	for( uint64_t i = 0; i < wm -> zone_limit; i++ ) {
		// object was assigned to zone?
		if( ! zone[ i ].object ) continue;	// no

		// copy the contents of the buffer to the memory space of the graphics card
		uint32_t *source = (uint32_t *) ((uintptr_t) wm -> canvas.descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ));
		for( uint64_t y = zone[ i ].y; y < zone[ i ].y + zone[ i ].height; y++ )
			for( uint64_t x = zone[ i ].x; x < zone[ i ].x + zone[ i ].width; x++ )
				wm -> framebuffer.base_address[ (y * (wm -> framebuffer.pitch_byte >> STD_VIDEO_DEPTH_shift)) + x ] = source[ (y * wm -> canvas.width) + x ] & ~STD_COLOR_mask;
	}

	// all zones parsed
	wm -> zone_limit = EMPTY;
}
