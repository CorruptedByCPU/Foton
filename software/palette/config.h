/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	WINDOW_WIDTH	(LIB_UI_MARGIN_DEFAULT + CANVAS_WIDTH + LIB_UI_PADDING_DEFAULT + SPECTRUM_WIDTH + LIB_UI_PADDING_DEFAULT + SAMPLE_WIDTH + LIB_UI_MARGIN_DEFAULT)
#define	WINDOW_HEIGHT	(LIB_UI_HEADER_HEIGHT + CANVAS_HEIGHT + LIB_UI_MARGIN_DEFAULT)

#define	CANVAS_WIDTH	256
#define	CANVAS_HEIGHT	256

#define	SPECTRUM_WIDTH	CANVAS_WIDTH / 10

#define	SAMPLE_WIDTH	108
#define	SAMPLE_HEIGHT	50

void palette_ui( void );
