/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void tiwyn_fill( void ) {
	// properties of zone list
	struct TIWYN_STRUCTURE_ZONE *zone = tiwyn_zone_base_address;

	// fill every zone with assigned object
	for( uint64_t i = 0; i < tiwyn_zone_limit; i++ ) {
		// object assigned to zone?
		if( ! zone[ i ].object ) continue;	// no

		// fill zone with selected object
		uint32_t *source = (uint32_t *) ((uintptr_t) zone[ i ].object -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));
		uint32_t *target = (uint32_t *) ((uintptr_t) tiwyn_object_cache.descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));
		for( uint64_t y = zone[ i ].y; y < zone[ i ].height + zone[ i ].y; y++ )
			for( uint64_t x = zone[ i ].x; x < zone[ i ].width + zone[ i ].x; x++ ) {
				// color properties
				uint32_t color_current = target[ (y * tiwyn_object_cache.width) + x ];
				uint32_t color_new = source[ (x - zone[ i ].object -> x) + (zone[ i ].object -> width * (y - zone[ i ].object -> y)) ];

				// perform the operation based on the alpha channel
				switch( color_new >> 24 ) {
					// transparent
					case 0x00: { break; }
		
					// opaque
					case (uint8_t) 0xFF: { target[ (y * tiwyn_object_cache.width) + x ] = color_new; break; }

					// calculate the color based on the alpha channel
					default: { target[ (y * tiwyn_object_cache.width) + x ] = lib_color_blend( color_current, color_new ); }
				}
			}

		// synchronize workbench with framebuffer
		tiwyn_object_cache.descriptor -> flags |= STD_WINDOW_FLAG_flush;
	}

	// all zones filled
	tiwyn_zone_limit = EMPTY;
}
