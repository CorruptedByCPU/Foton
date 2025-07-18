/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_fill( void ) {
	// fill every zone with assigned object
	for( uint64_t i = 0; i < wm_zone_limit; i++ ) {
		// object assigned to zone?
		if( ! wm_zone_base_address[ i ].object ) continue;	// no

		// fill zone with selected object
		uint32_t *source = (uint32_t *) ((uintptr_t) wm_zone_base_address[ i ].object -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));
		uint32_t *target = (uint32_t *) ((uintptr_t) wm_object_cache.descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));
		for( uint64_t y = wm_zone_base_address[ i ].y; y < wm_zone_base_address[ i ].height + wm_zone_base_address[ i ].y; y++ )
			for( uint64_t x = wm_zone_base_address[ i ].x; x < wm_zone_base_address[ i ].width + wm_zone_base_address[ i ].x; x++ ) {
				// color properties
				uint32_t color_current = target[ (y * wm_object_cache.width) + x ];
				uint32_t color_new = source[ (x - wm_zone_base_address[ i ].object -> x) + (wm_zone_base_address[ i ].object -> width * (y - wm_zone_base_address[ i ].object -> y)) ];

				// perform the operation based on the alpha channel
				switch( color_new >> 24 ) {
					// transparent
					case 0x00: { break; }
		
					// opaque
					case (uint8_t) 0xFF: { target[ (y * wm_object_cache.width) + x ] = color_new; break; }

					// calculate the color based on the alpha channel
					default: { target[ (y * wm_object_cache.width) + x ] = lib_color_blend( color_current, color_new ); }
				}
			}

		// synchronize workbench with framebuffer
		wm_object_cache.descriptor -> flags |= STD_WINDOW_FLAG_flush;
	}

	// all zones filled
	wm_zone_limit = EMPTY;
}
