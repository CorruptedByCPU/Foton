/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/interface.h"
	#include	"../library/random.h"
	#include	"../library/rgl.h"
	#include	"../library/string.h"
	//----------------------------------------------------------------------
	// variables, routines, procedures
	//----------------------------------------------------------------------
	#include	"./taris/config.h"
	#include	"./taris/data.c"
	#include	"./taris/random.c"
	#include	"./taris/collision.c"
	#include	"./taris/init.c"
	#include	"./taris/draw.c"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize gameplay
	taris_init();

	uint64_t level = 0;

	// restart game loop
	while( TRUE ) {
		// draw a new block
		taris_random();

		// removed lines at collision
		uint8_t removed = 0;

		// playgame loop
		while( taris_play ) {
			// move block one level lower
			taris_brick_selected_y++;

			// check collision
			if( taris_collision( taris_brick_selected_x, taris_brick_selected_y ) ) {
				// collision at startpoint?
				if( taris_brick_selected_y == 0 ) taris_play = FALSE;
				else {
					// undo block movement
					taris_brick_selected_y--;

					// for every line of brick
					for( uint8_t n = 0; n < TARIS_BRICK_WIDTH_bit; n++ ) {
						// sync with playground
						uint16_t block_line = ((taris_brick_selected >> (n * 4)) & 0x0F) << taris_brick_selected_x;
						if( block_line ) taris_playground[ taris_brick_selected_y + n ] |= block_line;

						// and preserve color
						for( uint8_t c = 0; c < TARIS_BRICK_WIDTH_bit; c++ ) if( taris_brick_selected >> (n * 4) & 1 << c ) taris_playground_color[ ((taris_brick_selected_y + n) * TARIS_PLAYGROUND_WIDTH_bit) + (taris_brick_selected_x + c) ] = taris_brick_selected_id;
					}

					// remove every full line
					for( uint8_t i = 1; i < TARIS_PLAYGROUND_HEIGHT_brick - 1; i++ ) if( taris_playground[ i ] == TARIS_PLAYGROUND_full ) {
						// and remove it
						for( uint8_t j = i; j >= 1; j-- ) {
							// line
							taris_playground[ j ] = taris_playground[ j - 1 ];

							// and color
							for( uint8_t c = 0; c < TARIS_PLAYGROUND_WIDTH_bit; c++ )
								taris_playground_color[ (j * TARIS_PLAYGROUND_WIDTH_bit) + c ] = taris_playground_color[ ((j - 1) * TARIS_PLAYGROUND_WIDTH_bit) + c ];
						}

						// removed lines
						removed++;
					}
				}

				// // if removed some lines
				// if( removed ) {
				// 	// increase overall lines
				// 	lines += removed;

				// 	// calculate scored points
				// 	points += taris_score[ removed - 1 ] * (level + 1);

				// 	// increase difficulty?
				// 	if( lines >= taris_difficult[ level ] ) level++;
				// }

				// next block
				break;
			}

			// wait for next movement
			uint64_t wait = std_microtime() + taris_speed[ level ];
			do {
				// recieve key
				uint16_t key = getkey();

				// exit game?
				if( key == 'q' ) return 0;	// yes

				// move left?
				if( key == STD_KEY_ARROW_LEFT && ! taris_collision( taris_brick_selected_x - 1, taris_brick_selected_y ) ) taris_brick_selected_x--;

				// move right?
				if( key == STD_KEY_ARROW_RIGHT && ! taris_collision( taris_brick_selected_x + 1, taris_brick_selected_y ) ) taris_brick_selected_x++;

				// rotate left?
				if( key == 'z' ) {
					taris_brick_selected = (taris_brick_selected >> (64 - 16)) | (taris_brick_selected << 16);
					if( taris_collision( taris_brick_selected_x, taris_brick_selected_y ) ) taris_brick_selected = (taris_brick_selected >> 16) | (taris_brick_selected << (64 - 16));
				}

				// rotate right?
				if( key == 'x' ) {
					taris_brick_selected = (taris_brick_selected >> 16) | (taris_brick_selected << (64 - 16));
					if( taris_collision( taris_brick_selected_x, taris_brick_selected_y ) ) taris_brick_selected = (taris_brick_selected >> (64 - 16)) | (taris_brick_selected << 16);
				}

				// hard drop?
				if( key == STD_KEY_ARROW_UP ) {
					// move block down until collision
					while( ! taris_collision( taris_brick_selected_x, taris_brick_selected_y + 1 ) ) taris_brick_selected_y++;

					// next block
					break;
				}

				// show current game status
				taris_draw();
			} while( wait > std_microtime() );
		}

		// if end of game
		if( ! taris_play ) {
			// wait for restart of game
			while( getkey() != STD_ASCII_SPACE );

			// clean up playground
			for( uint8_t i = 0; i < TARIS_PLAYGROUND_HEIGHT_brick - 1; i++ )
				taris_playground[ i ] = TARIS_PLAYGROUND_empty;

			// start new round
			taris_play = TRUE;
		}
	}

	// end of execution
	return 0;
}