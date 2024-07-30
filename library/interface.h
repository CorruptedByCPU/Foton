/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_INTERFACE
	#define	LIB_INTERFACE

	#ifndef	LIB_FONT
		#include	"./font.h"
	#endif
	#ifndef	LIB_IMAGE
		#include	"./image.h"
	#endif
	#ifndef	LIB_JSON
		#include	"./json.h"
	#endif
	#ifndef	LIB_STRING
		#include	"./string.h"
	#endif

	#define	LIB_INTERFACE_GLOBAL_NAME_limit			64

	#define	LIB_INTERFACE_HEADER_HEIGHT_pixel		(LIB_FONT_HEIGHT_pixel + 8)	// 4 pixels from above and under
	#define	LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel		LIB_INTERFACE_HEADER_HEIGHT_pixel

	#define	LIB_INTERFACE_ELEMENT_TYPE_null			0x00
	#define	LIB_INTERFACE_ELEMENT_TYPE_label		0x01
	#define	LIB_INTERFACE_ELEMENT_TYPE_button		0x02
	#define	LIB_INTERFACE_ELEMENT_TYPE_control_close	0x03
	#define	LIB_INTERFACE_ELEMENT_TYPE_control_maximize	0x04
	#define	LIB_INTERFACE_ELEMENT_TYPE_control_minimize	0x05
	#define	LIB_INTERFACE_ELEMENT_TYPE_menu			0x06

	#define	LIB_INTERFACE_ELEMENT_FLAG_hover		0b00000001

	#define	LIB_INTERFACE_BORDER_pixel			1
	#define	LIB_INTERFACE_BORDER_COLOR_default		0xFF383838
	#define	LIB_INTERFACE_BORDER_COLOR_inactive		0xFF181818

	#define	LIB_INTERFACE_COLOR_background			0xFF141414
	#define	LIB_INTERFACE_COLOR_background_dark		0xFF080808
	#define	LIB_INTERFACE_COLOR_foreground			0xFFF0F0F0
	#define	LIB_INTERFACE_COLOR_background_light		0xFF282828
	#define	LIB_INTERFACE_COLOR_background_hover		0xFF208020

	#define	LIB_INTERFACE_COLOR_MENU_background_hover	0xFF0C0C0C

	#define	LIB_INTERFACE_NAME_limit			LIB_INTERFACE_GLOBAL_NAME_limit

	struct LIB_INTERFACE_STRUCTURE {
		struct STD_WINDOW_STRUCTURE_DESCRIPTOR	*descriptor;
		uint8_t		*properties;
		//--------------------------------------------------
		int16_t		x;
		int16_t		y;
		uint16_t	width;
		uint16_t	height;
		//--------------------------------------------------
		int16_t		previous_x;
		int16_t		previous_y;
		uint16_t	previous_width;
		uint16_t	previous_height;
		//--------------------------------------------------
		uint16_t	min_width;
		uint16_t	min_height;
		//--------------------------------------------------
		uint8_t		controls;
		uint8_t		active_semaphore;
		uint32_t	background_color;	// if set (alpha channel set), choose as background color
		//--------------------------------------------------
		uint8_t		name_length;
		uint8_t		name[ LIB_INTERFACE_NAME_limit ];
	};

	struct	LIB_INTERFACE_STRUCTURE_ELEMENT {
		uint8_t		type;
		uint8_t		flags;
		uint16_t	size_byte;	// element size (depends of type)
		int16_t		x;
		int16_t		y;
		uint16_t	width;
		uint16_t	height;
		uint16_t	id;
		void		(*function)( void );
	};

	struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL {
		struct LIB_INTERFACE_STRUCTURE_ELEMENT	control;
		void	(*event)( void );
	};

	struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON {
		struct LIB_INTERFACE_STRUCTURE_ELEMENT	label_or_button;
		uint16_t	name_length;
		uint8_t		*name;
	};

	struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU {
		struct LIB_INTERFACE_STRUCTURE_ELEMENT	menu;
		void		(*event)( struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *menu );
		uint16_t	name_length;
		uint8_t		*name;
		uint8_t		*command;
		uint32_t	*icon;
	};

	// properties of Interface assigned to Window
	void lib_interface( struct LIB_INTERFACE_STRUCTURE *interface );

	// fill interface background with default colors
	void lib_interface_clear( struct LIB_INTERFACE_STRUCTURE *interface );

	// changes JSON format to internal Interface format
	void lib_interface_convert( struct LIB_INTERFACE_STRUCTURE *interface );

	// parse all elements of interface and show them
	void lib_interface_draw( struct LIB_INTERFACE_STRUCTURE *interface );

	// find element properties by its ID
	uintptr_t lib_interface_element_by_id( struct LIB_INTERFACE_STRUCTURE *interface, uint64_t id );

	// show control element of definied properties
	void lib_interface_element_control( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *control );

	// show label element of definied properties
	void lib_interface_element_label( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *element );

	// show menu element of definied properties
	void lib_interface_element_menu( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *element );

	// check incomming events
	struct LIB_INTERFACE_STRUCTURE *lib_interface_event( struct LIB_INTERFACE_STRUCTURE *interface );
	// support function of lib_interface_event, performed exclusively by WM
	void lib_interface_event_handler( struct LIB_INTERFACE_STRUCTURE *interface );

	// change status of elements of interface
	void lib_interface_hover( struct LIB_INTERFACE_STRUCTURE *interface );

	// rename window header
	void lib_interface_name( struct LIB_INTERFACE_STRUCTURE *interface );

	// update name on window interface
	void lib_interface_name_rewrite( struct LIB_INTERFACE_STRUCTURE *interface );

	// create window space accoring to JSON specification
	uint8_t lib_interface_window( struct LIB_INTERFACE_STRUCTURE *interface );
#endif