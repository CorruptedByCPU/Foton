/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_cursor( void ) {
	// requested redraw?
	if( ! (wm -> cursor -> descriptor -> flags & LIB_WINDOW_FLAG_flush) ) return;	// no

	// remove current cursor position from workbench
	wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm -> cursor, FALSE );

	// assign objects to cursor zone and redraw on screen
	wm_zone(); wm_fill();

	// properties of areas
	uint32_t *source = (uint32_t *) ((uintptr_t) wm -> cursor -> descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ));
	uint32_t *target = (uint32_t *) ((uintptr_t) wm -> canvas.descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ));

	// calculate overflows
	uint16_t width = wm -> cursor -> width;
	uint16_t height = wm -> cursor -> height;
	if( wm -> cursor -> x + wm -> cursor -> width > wm -> canvas.width ) width -= (wm -> cursor -> x + wm -> cursor -> width) - wm -> canvas.width;
	if( wm -> cursor -> y + wm -> cursor -> height > wm -> canvas.height ) height -= (wm -> cursor -> y + wm -> cursor -> height) - wm -> canvas.height;

	// fill zone with current object
	for( uint64_t y = 0; y < height; y++ )
		for( uint64_t x = 0; x < width; x++ ) {
			// calculate position of both pixels
			uint32_t *source_pixel = (uint32_t *) &source[ (y * wm -> cursor -> width) + x ];
			uint32_t *target_pixel = (uint32_t *) &target[ ((y + wm -> cursor -> y) * wm -> canvas.width) + (x + wm -> cursor -> x) ];

			// perform the operation based on the alpha channel
			switch( *source_pixel >> 24 ) {
				// transparent
				case 0x00: { break; }
		
				// opaque
				case (uint8_t) 0xFF: { *target_pixel = *source_pixel; break; }

				// calculate the color based on the alpha channel
				default: { *target_pixel = lib_color_blend( *target_pixel, *source_pixel ); }
			}
		}

	// cursor parsed
	wm -> cursor -> descriptor -> flags ^= LIB_WINDOW_FLAG_flush;
}
