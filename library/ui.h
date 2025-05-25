/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_FONT
	#include	"../library/font.h"
#endif
#ifndef	LIB_STRING
	#include	"../library/string.h"
#endif
#ifndef	LIB_WINDOW
	#include	"./window.h"
#endif

#define	LIB_UI_COLOR_DEFAULT			0xFFFFFFFF
#define	LIB_UI_COLOR_INCREASE			0x00202020
#define	LIB_UI_COLOR_INCREASE_LIGHT		0x00080808
#define	LIB_UI_COLOR_INPUT			0xFFF0F0F0
#define	LIB_UI_COLOR_INPUT_DISABLED		0xFF808080
#define	LIB_UI_COLOR_CHECKBOX_SELECTED		LIB_UI_COLOR_BACKGROUND_BUTTON
#define	LIB_UI_COLOR_RADIO_SELECTED		LIB_UI_COLOR_BACKGROUND_BUTTON

#define	LIB_UI_COLOR_BACKGROUND_DEFAULT		0xFF181818
#define	LIB_UI_COLOR_BACKGROUND_BUTTON		0xFF00CC00
#define	LIB_UI_COLOR_BACKGROUND_BUTTON_DISABLED	0xFF404040
#define	LIB_UI_COLOR_BACKGROUND_CHECKBOX	(LIB_UI_COLOR_BACKGROUND_DEFAULT + 0x00101010)
#define	LIB_UI_COLOR_BACKGROUND_CONTROL_CLOSE	LIB_UI_COLOR_BACKGROUND_BUTTON
#define	LIB_UI_COLOR_BACKGROUND_CONTROL_DEFAULT	(LIB_UI_COLOR_BACKGROUND_DEFAULT + LIB_UI_COLOR_INCREASE_LIGHT)
#define	LIB_UI_COLOR_BACKGROUND_INPUT		0xFF000000
#define	LIB_UI_COLOR_BACKGROUND_INPUT_DISABLED	0xFF040404
#define	LIB_UI_COLOR_BACKGROUND_INPUT_ACTIVE	0xFF080808
#define	LIB_UI_COLOR_BACKGROUND_RADIO		LIB_UI_COLOR_BACKGROUND_CHECKBOX
#define	LIB_UI_COLOR_BACKGROUND_SHADOW		LIB_UI_COLOR_BACKGROUND_DEFAULT - LIB_UI_COLOR_INCREASE_LIGHT

#define	LIB_UI_ELEMENT_CONTROL_TYPE_close	0x01
#define	LIB_UI_ELEMENT_CONTROL_TYPE_max		0x02
#define	LIB_UI_ELEMENT_CONTROL_TYPE_min		0x04

#define	LIB_UI_ELEMENT_FLAG_hover		0x01
#define	LIB_UI_ELEMENT_FLAG_set			0x02
#define	LIB_UI_ELEMENT_FLAG_active		0x04
#define	LIB_UI_ELEMENT_FLAG_disabled		0x80

#define	LIB_UI_HEADER_HEIGHT			22

#define	LIB_UI_ELEMENT_BUTTON_height		LIB_UI_HEADER_HEIGHT
#define	LIB_UI_ELEMENT_CHECKBOX_height		LIB_FONT_HEIGHT_pixel
#define	LIB_UI_ELEMENT_INPUT_height		20
#define	LIB_UI_ELEMENT_LABEL_height		LIB_FONT_HEIGHT_pixel
#define	LIB_UI_ELEMENT_RADIO_height		LIB_FONT_HEIGHT_pixel

#define	LIB_UI_MARGIN_DEFAULT		5
#define	LIB_UI_PADDING_DEFAULT		4

#define	LIB_UI_RADIUS_DEFAULT		1

enum LIB_UI_ELEMENT_TYPE {
	NONE,
	BUTTON,
	CHECKBOX,
	CONTROL,
	INPUT,
	LABEL,
	RADIO
};

struct LIB_UI_STRUCTURE {
	struct LIB_WINDOW_STRUCTURE			*window;

	struct LIB_UI_STRUCTURE_ELEMENT			**element;

	struct LIB_UI_STRUCTURE_ELEMENT_BUTTON		**button;
	struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX	**checkbox;
	struct LIB_UI_STRUCTURE_ELEMENT_CONTROL		**control;
	struct LIB_UI_STRUCTURE_ELEMENT_INPUT		**input;
	struct LIB_UI_STRUCTURE_ELEMENT_LABEL		**label;
	struct LIB_UI_STRUCTURE_ELEMENT_RADIO		**radio;

	uint64_t					element_active;

	uint64_t					limit;
	uint64_t					limit_button;
	uint64_t					limit_checkbox;
	uint64_t					limit_control;
	uint64_t					limit_input;
	uint64_t					limit_label;
	uint64_t					limit_radio;

	struct	STD_STRUCTURE_KEYBOARD_STATE		keyboard;
};

struct LIB_UI_STRUCTURE_ELEMENT {
	uint16_t			x;
	uint16_t			y;
	uint16_t			width;
	uint16_t			height;
	enum LIB_UI_ELEMENT_TYPE	type;
	uint8_t				flag;
	uint8_t				*name;
};

struct LIB_UI_STRUCTURE_ELEMENT_BUTTON {
	struct LIB_UI_STRUCTURE_ELEMENT	standard;
};

struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX {
	struct LIB_UI_STRUCTURE_ELEMENT	standard;
	uint8_t				set;
};

struct LIB_UI_STRUCTURE_ELEMENT_CONTROL {
	struct LIB_UI_STRUCTURE_ELEMENT	standard;
	uint8_t				type;
};

struct LIB_UI_STRUCTURE_ELEMENT_INPUT {
	struct LIB_UI_STRUCTURE_ELEMENT	standard;
	uint64_t			offset;
	uint64_t			index;
};

struct LIB_UI_STRUCTURE_ELEMENT_LABEL {
	struct LIB_UI_STRUCTURE_ELEMENT	standard;
};

struct LIB_UI_STRUCTURE_ELEMENT_RADIO {
	struct LIB_UI_STRUCTURE_ELEMENT	standard;
	uint8_t				set;
	uint8_t				group;
};

struct LIB_UI_STRUCTURE *lib_ui( struct LIB_WINDOW_STRUCTURE *window );
uint64_t lib_ui_add_button( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name, uint16_t height, uint8_t flag );
uint64_t lib_ui_add_checkbox( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name, uint8_t flag );
uint64_t lib_ui_add_control( struct LIB_UI_STRUCTURE *ui, uint8_t type );
uint64_t lib_ui_add_input( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name, uint8_t flag );
uint64_t lib_ui_add_label( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name );
uint64_t lib_ui_add_radio( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name, uint8_t group, uint8_t flag );
void lib_ui_clean( struct LIB_UI_STRUCTURE *ui );
void lib_ui_event( struct LIB_UI_STRUCTURE *ui );
static void lib_ui_event_keyboard( struct LIB_UI_STRUCTURE *ui, uint8_t *sync );
static void lib_ui_event_mouse( struct LIB_UI_STRUCTURE *ui, uint8_t *sync );
void lib_ui_flush( struct LIB_UI_STRUCTURE *ui );
static void lib_ui_list_insert( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT *element );
void lib_ui_show_button( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_BUTTON *button );
void lib_ui_show_checkbox( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX *checkbox );
void lib_ui_show_control( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_CONTROL *control );
void lib_ui_show_element( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT *element );
void lib_ui_show_input( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_INPUT *input );
void lib_ui_show_label( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_LABEL *label );
void lib_ui_show_name( struct LIB_UI_STRUCTURE *ui );
void lib_ui_show_radio( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_RADIO *radio );
