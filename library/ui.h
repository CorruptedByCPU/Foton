/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_WINDOW
	#include	"./window.h"
#endif

#define	LIB_UI_MARGIN_DEFAULT	5

struct LIB_UI_STRUCTURE {
	struct LIB_WINDOW_STRUCTURE			*window;
	struct LIB_UI_STRUCTURE_ELEMENT_LABEL		**label;

	uint64_t					limit_label;
};

struct LIB_UI_STRUCTURE_ELEMENT {
	uint16_t	x;
	uint16_t	y;
};

struct LIB_UI_STRUCTURE_ELEMENT_LABEL {
	struct LIB_UI_STRUCTURE_ELEMENT	standard;
	uint8_t				*name;
};

struct LIB_UI_STRUCTURE *lib_ui( struct LIB_WINDOW_STRUCTURE *window );
uint64_t lib_ui_add_label( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint8_t *name );
void lib_ui_sync( struct LIB_UI_STRUCTURE *ui );
