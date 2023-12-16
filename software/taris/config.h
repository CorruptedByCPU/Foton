/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	#define	TARIS_MENU_HEIGHT_pixel		(LIB_FONT_HEIGHT_pixel << STATIC_MULTIPLE_BY_2_shift) + 4

	#define	TARIS_BRICK_START_POSITION_x	6
	#define	TARIS_BRICK_START_POSITION_y	-1

	#define	TARIS_BRICK_PADDING_pixel	1
	#define	TARIS_BRICK_WIDTH_bit		4
	#define	TARIS_BRICK_WIDTH_pixel		16
	#define	TARIS_BRICK_HEIGHT_pixel	16

	#define	TARIS_PLAYGROUND_empty		0b1110000000000111
	#define	TARIS_PLAYGROUND_full		0b1111111111111111

	#define	TARIS_PLAYGROUND_WIDTH_bit	16
	#define	TARIS_PLAYGROUND_WIDTH_brick	10
	#define	TARIS_PLAYGROUND_HEIGHT_brick	22	// top + bottom

	#define	TARIS_PLAYGROUND_WIDTH_pixel	((TARIS_PLAYGROUND_WIDTH_brick * (TARIS_BRICK_WIDTH_pixel + TARIS_BRICK_PADDING_pixel)) - TARIS_BRICK_PADDING_pixel)
	#define	TARIS_PLAYGROUND_HEIGHT_pixel	((TARIS_PLAYGROUND_HEIGHT_brick - 2) * (TARIS_BRICK_HEIGHT_pixel + 1)) - 1

	#define	TARIS_WINDOW_WIDTH_pixel	(1 + TARIS_PLAYGROUND_WIDTH_pixel + 1)
	#define	TARIS_WINDOW_HEIGHT_pixel	(LIB_INTERFACE_HEADER_HEIGHT_pixel + TARIS_MENU_HEIGHT_pixel + 1 + TARIS_PLAYGROUND_HEIGHT_pixel)

	void close( void );