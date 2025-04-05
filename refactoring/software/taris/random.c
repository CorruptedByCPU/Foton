/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void taris_random( void ) {
	// draw a block id
	taris_brick_selected_id = lib_random() % 7;

	// set block type
	taris_brick_selected = taris_brick[ taris_brick_selected_id ];
	taris_brick_selected_color = taris_color[ taris_brick_selected_id ];

	// initial block position
	taris_brick_selected_x = TARIS_BRICK_START_POSITION_x;
	taris_brick_selected_y = TARIS_BRICK_START_POSITION_y;
}