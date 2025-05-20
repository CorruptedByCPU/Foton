/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_WINDOW
	#include	"./window.h"
#endif

#define	LIB_UI_FLAG_hover		0x01

#define	LIB_UI_COLOR_DEFAULT			0xFFFFFFFF
#define	LIB_UI_COLOR_INCREASE			0x00202020
#define	LIB_UI_COLOR_BACKGROUND_DEFAULT		0xFF202020
#define	LIB_UI_COLOR_BACKGROUND_BUTTON		0xFF00CC00
#define	LIB_UI_COLOR_BACKGROUND_CHECKBOX	(LIB_UI_COLOR_BACKGROUND_DEFAULT + 0x00101010)
#define	LIB_UI_COLOR_BACKGROUND_RADIO		LIB_UI_COLOR_BACKGROUND_CHECKBOX

#define	LIB_UI_HEIGHT_DEFAULT		16

#define	LIB_UI_MARGIN_DEFAULT		5
#define	LIB_UI_PADDING_DEFAULT		4

#define	LIB_UI_RADIUS_DEFAULT		1

struct LIB_UI_STRUCTURE {
	struct LIB_WINDOW_STRUCTURE			*window;
	struct LIB_UI_STRUCTURE_ELEMENT_BUTTON		**button;
	struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX	**checkbox;
	struct LIB_UI_STRUCTURE_ELEMENT_LABEL		**label;
	struct LIB_UI_STRUCTURE_ELEMENT_RADIO		**radio;

	uint64_t					limit_button;
	uint64_t					limit_checkbox;
	uint64_t					limit_label;
	uint64_t					limit_radio;
};

struct LIB_UI_STRUCTURE_ELEMENT {
	uint16_t	x;
	uint16_t	y;
	uint16_t	width;
	uint16_t	height;
	uint8_t		flag;
};

struct LIB_UI_STRUCTURE_ELEMENT_BUTTON {
	struct LIB_UI_STRUCTURE_ELEMENT	standard;
	uint8_t				*name;
};

struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX {
	struct LIB_UI_STRUCTURE_ELEMENT	standard;
	uint8_t				*name;
	uint8_t				set;
};

struct LIB_UI_STRUCTURE_ELEMENT_LABEL {
	struct LIB_UI_STRUCTURE_ELEMENT	standard;
	uint8_t				*name;
};

struct LIB_UI_STRUCTURE_ELEMENT_RADIO {
	struct LIB_UI_STRUCTURE_ELEMENT	standard;
	uint8_t				*name;
	uint8_t				set;
	uint8_t				group;
};

struct LIB_UI_STRUCTURE *lib_ui( struct LIB_WINDOW_STRUCTURE *window );
uint64_t lib_ui_add_button( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *name );
uint64_t lib_ui_add_checkbox( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name );
uint64_t lib_ui_add_label( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint8_t *name );
uint64_t lib_ui_add_radio( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t group, uint8_t *name );
void lib_ui_clean( struct LIB_UI_STRUCTURE *ui );
void lib_ui_event( struct LIB_UI_STRUCTURE *ui );
void lib_ui_show_button( struct LIB_UI_STRUCTURE *ui, uint64_t id );
void lib_ui_show_checkbox( struct LIB_UI_STRUCTURE *ui, uint64_t id );
void lib_ui_show_label( struct LIB_UI_STRUCTURE *ui, uint64_t id );
void lib_ui_show_radio( struct LIB_UI_STRUCTURE *ui, uint64_t id );
