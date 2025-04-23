/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t taris_collision( int64_t x, int64_t y ) {
	// for every line of brick
	for( uint8_t n = 0; n < TARIS_BRICK_WIDTH_bit; n++ ) {
		// n'line of playground
		uint16_t playground_line = taris_playground[ y ];

		// n'line of brick
		uint16_t brick_line = (((taris_brick_selected >> (n * TARIS_BRICK_WIDTH_bit)) & 0x0F) << x);

		// collision?
		if( brick_line & playground_line ) return TRUE;

		y++;
	}

	// no collision
	return FALSE;
}
