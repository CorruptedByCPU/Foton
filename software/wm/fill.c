/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_fill( void ) {
	// properties of zone list
	struct WM_STRUCTURE_ZONE *zone = wm -> zone;

	// properties of object list
	struct WM_STRUCTURE_OBJECT **list = wm -> list;

	// for each object from bottom
	for( uint64_t j = wm -> list_limit; j > 0; j-- ) {
		// fill every zone with assigned object
		for( uint64_t i = 0; i < wm -> zone_limit; i++ ) {
			if( list[ j - 1 ] != zone[ i ].object ) continue;

			// fill zone with selected object
			uint32_t *source = (uint32_t *) ((uintptr_t) zone[ i ].object -> descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ));
			uint32_t *target = (uint32_t *) ((uintptr_t) wm -> canvas.descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ));
			for( uint64_t y = zone[ i ].y; y < zone[ i ].height + zone[ i ].y; y++ )
				for( uint64_t x = zone[ i ].x; x < zone[ i ].width + zone[ i ].x; x++ ) {
					uint32_t color_new = source[ (x - zone[ i ].object -> x) + (zone[ i ].object -> width * (y - zone[ i ].object -> y)) ];

					// color properties
					uint32_t color_current = target[ (y * wm -> canvas.width) + x ];

					// perform the operation based on the alpha channel
					switch( color_new >> 24 ) {
						// transparent
						case 0x00: { continue; }
		
						// opaque
						case (uint8_t) 0xFF: { target[ (y * wm -> canvas.width) + x ] = color_new; break; }

						// calculate the color based on the alpha channel
						default: { target[ (y * wm -> canvas.width) + x ] = lib_color_blend( color_current, color_new ); }
					}
				}
		}
	}
}
