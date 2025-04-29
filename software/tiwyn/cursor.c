/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void tiwyn_cursor( void ) {
	// requested redraw?
	if( ! (tiwyn_object_cursor -> descriptor -> flags & STD_WINDOW_FLAG_flush) ) return;	// no

	// remove current cursor position from workbench
	tiwyn_zone_insert( (struct TIWYN_STRUCTURE_ZONE *) tiwyn_object_cursor, FALSE );

	// assign objects to cursor zone and redraw on screen
	tiwyn_zone(); tiwyn_fill();

	// properties of areas
	uint32_t *source = (uint32_t *) ((uintptr_t) tiwyn_object_cursor -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));
	uint32_t *target = (uint32_t *) ((uintptr_t) tiwyn_object_cache.descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));

	// calculate overflows
	uint16_t width = tiwyn_object_cursor -> width;
	uint16_t height = tiwyn_object_cursor -> height;
	if( tiwyn_object_cursor -> x + tiwyn_object_cursor -> width > tiwyn_object_cache.width ) width -= (tiwyn_object_cursor -> x + tiwyn_object_cursor -> width) - tiwyn_object_cache.width;
	if( tiwyn_object_cursor -> y + tiwyn_object_cursor -> height > tiwyn_object_cache.height ) height -= (tiwyn_object_cursor -> y + tiwyn_object_cursor -> height) - tiwyn_object_cache.height;

	// fill zone with current object
	for( uint64_t y = 0; y < height; y++ )
		for( uint64_t x = 0; x < width; x++ ) {
			// calculate position of both pixels
			uint32_t *source_pixel = (uint32_t *) &source[ (y * tiwyn_object_cursor -> width) + x ];
			uint32_t *target_pixel = (uint32_t *) &target[ ((y + tiwyn_object_cursor -> y) * tiwyn_object_cache.width) + (x + tiwyn_object_cursor -> x) ];

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
	tiwyn_object_cursor -> descriptor -> flags ^= STD_WINDOW_FLAG_flush;

	// synchronize workbench with framebuffer
	tiwyn_object_cache.descriptor -> flags |= STD_WINDOW_FLAG_flush;
}
