/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_fill( void ) {
	// properties of zone list
	struct WM_STRUCTURE_ZONE *zone = wm -> zone;

	// fill every zone with assigned object
	for( uint64_t i = 0; i < wm -> zone_limit; i++ ) {
		// object assigned to zone?
		if( ! zone[ i ].object ) continue;	// no

		// fill zone with selected object
		uint32_t *source = (uint32_t *) ((uintptr_t) zone[ i ].object -> descriptor + sizeof( struct LIB_WINDOW_STRUCTURE_DESCRIPTOR ));
		uint32_t *target = (uint32_t *) ((uintptr_t) wm -> canvas.descriptor + sizeof( struct LIB_WINDOW_STRUCTURE_DESCRIPTOR ));
		for( uint64_t y = zone[ i ].y; y < zone[ i ].height + zone[ i ].y; y++ )
			for( uint64_t x = zone[ i ].x; x < zone[ i ].width + zone[ i ].x; x++ ) {
				// color properties
				uint32_t color_current = target[ (y * wm -> canvas.width) + x ];
				uint32_t color_new = source[ (x - zone[ i ].object -> x) + (zone[ i ].object -> width * (y - zone[ i ].object -> y)) ];

				// perform the operation based on the alpha channel
				switch( color_new >> 24 ) {
					// transparent
					case 0x00: { break; }
		
					// opaque
					case (uint8_t) 0xFF: { target[ (y * wm -> canvas.width) + x ] = color_new; break; }

					// calculate the color based on the alpha channel
					default: { target[ (y * wm -> canvas.width) + x ] = lib_color_blend( color_current, color_new ); }
				}
			}
	}

	// all zones filled
	wm -> zone_limit = EMPTY;
}
