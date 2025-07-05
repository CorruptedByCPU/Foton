/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_fill( void ) {
	// properties of object list
	struct WM_STRUCTURE_OBJECT **list = wm -> list;

	// sort zones from bottom to top
	uint64_t limit = 0;
	struct WM_STRUCTURE_ZONE *tmp = (struct WM_STRUCTURE_ZONE *) malloc( sizeof( struct WM_STRUCTURE_ZONE ) * wm -> zone_limit );

	// according to list
	for( uint64_t i = wm -> list_limit; i > 0; i-- ) for( uint64_t z = 0; z < wm -> zone_limit; z++ ) if( wm -> zone[ z ].object == wm -> list[ i - 1 ] ) tmp[ limit++ ] = wm -> zone[ z ];

	// fill every zone with assigned object
	for( uint64_t i = 0; i < limit; i++ ) {
		// fill zone with selected object
		uint32_t *source = (uint32_t *) ((uintptr_t) tmp[ i ].object -> descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ));
		uint32_t *target = (uint32_t *) ((uintptr_t) wm -> canvas.descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ));

		// connect pixels
		for( uint64_t y = tmp[ i ].y; y < tmp[ i ].height + tmp[ i ].y; y++ )
			for( uint64_t x = tmp[ i ].x; x < tmp[ i ].width + tmp[ i ].x; x++ ) {
				uint32_t color_new = source[ (x - tmp[ i ].object -> x) + (tmp[ i ].object -> width * (y - tmp[ i ].object -> y)) ];

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

	// release temporary list of zones
	free( tmp );

}
