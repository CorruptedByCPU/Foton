/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void tiwyn_cursor( void ) {
	// requested redraw?
	if( ! (tiwyn -> cursor -> descriptor -> flags & STD_WINDOW_FLAG_flush) ) return;	// no

	// remove current cursor position from workbench
	tiwyn_zone_insert( (struct TIWYN_STRUCTURE_ZONE *) tiwyn -> cursor, FALSE );

	// assign objects to cursor zone and redraw on screen
	tiwyn_zone(); tiwyn_fill();

	// properties of areas
	uint32_t *source = (uint32_t *) ((uintptr_t) tiwyn -> cursor -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));
	uint32_t *target = (uint32_t *) ((uintptr_t) tiwyn -> canvas.descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));

	// calculate overflows
	uint16_t width = tiwyn -> cursor -> width;
	uint16_t height = tiwyn -> cursor -> height;
	if( tiwyn -> cursor -> x + tiwyn -> cursor -> width > tiwyn -> canvas.width ) width -= (tiwyn -> cursor -> x + tiwyn -> cursor -> width) - tiwyn -> canvas.width;
	if( tiwyn -> cursor -> y + tiwyn -> cursor -> height > tiwyn -> canvas.height ) height -= (tiwyn -> cursor -> y + tiwyn -> cursor -> height) - tiwyn -> canvas.height;

	// fill zone with current object
	for( uint64_t y = 0; y < height; y++ )
		for( uint64_t x = 0; x < width; x++ ) {
			// calculate position of both pixels
			uint32_t *source_pixel = (uint32_t *) &source[ (y * tiwyn -> cursor -> width) + x ];
			uint32_t *target_pixel = (uint32_t *) &target[ ((y + tiwyn -> cursor -> y) * tiwyn -> canvas.width) + (x + tiwyn -> cursor -> x) ];

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
	tiwyn -> cursor -> descriptor -> flags ^= STD_WINDOW_FLAG_flush;

	// synchronize workbench with framebuffer
	tiwyn -> canvas.descriptor -> flags |= STD_WINDOW_FLAG_flush;
}
