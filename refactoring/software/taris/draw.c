/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void taris_draw( void ) {
	// clean up playground
	lib_rgl_clean( taris_rgl );

	// show playground
	for( uint8_t y = 1; y < TARIS_PLAYGROUND_HEIGHT_brick - 1; y++ )
		for( uint8_t x = 3; x < 3 + TARIS_PLAYGROUND_WIDTH_brick; x++ )
			if( taris_playground[ y ] & 1 << x )
				lib_rgl_2d_square( taris_rgl, (x - 3) * (TARIS_BRICK_WIDTH_pixel + 1), (y - 1) * (TARIS_BRICK_HEIGHT_pixel + 1), TARIS_BRICK_WIDTH_pixel, TARIS_BRICK_HEIGHT_pixel, taris_color[ taris_playground_color[ (y * TARIS_PLAYGROUND_WIDTH_bit) + x ] ] );

	// calculate ghost position
	int taris_ghost_y = taris_brick_selected_y;
	while( ! taris_collision( taris_brick_selected_x, taris_ghost_y + 1 ) ) taris_ghost_y++;

	// show ghost
	for( int64_t y = 0; y < TARIS_BRICK_WIDTH_bit; y++ ) {
		// n'line of brick
		uint16_t brick_line = (taris_brick_selected >> (y * TARIS_BRICK_WIDTH_bit)) & 0x0F;

		for( int64_t x = 0; x < TARIS_BRICK_WIDTH_bit; x++ )
			if( brick_line & 1 << x ) lib_rgl_2d_square( taris_rgl, (x + (taris_brick_selected_x - 3)) * (TARIS_BRICK_WIDTH_pixel + 1), ((y - 1) + taris_ghost_y) * (TARIS_BRICK_HEIGHT_pixel + 1), TARIS_BRICK_WIDTH_pixel, TARIS_BRICK_HEIGHT_pixel, TARIS_GHOST_color );
	}

	// show player
	for( int64_t y = 0; y < TARIS_BRICK_WIDTH_bit; y++ ) {
		// n'line of brick
		uint16_t brick_line = (taris_brick_selected >> (y * TARIS_BRICK_WIDTH_bit)) & 0x0F;

		for( int64_t x = 0; x < TARIS_BRICK_WIDTH_bit; x++ )
			if( brick_line & 1 << x ) lib_rgl_2d_square( taris_rgl, (x + (taris_brick_selected_x - 3)) * (TARIS_BRICK_WIDTH_pixel + 1), ((y - 1) + taris_brick_selected_y) * (TARIS_BRICK_HEIGHT_pixel + 1), TARIS_BRICK_WIDTH_pixel, TARIS_BRICK_HEIGHT_pixel, taris_brick_selected_color );
	}

	// synchronize playground with window
	lib_rgl_flush( taris_rgl );

	// update window content on screen
	taris_interface.descriptor -> flags |= STD_WINDOW_FLAG_flush;
}
