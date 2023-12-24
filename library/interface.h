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

	#define	LIB_INTERFACE_HEADER_HEIGHT_pixel	(LIB_FONT_HEIGHT_pixel + 6)	// 3 pixels from above and under

	#define	LIB_INTERFACE_ELEMENT_TYPE_null		0x00
	#define	LIB_INTERFACE_ELEMENT_TYPE_label	0x01
	#define	LIB_INTERFACE_ELEMENT_TYPE_button	0x02

	#define	LIB_INTERFACE_BORDER_pixel		1

	#define	LIB_INTERFACE_COLOR_background		0xFF141414
	#define	LIB_INTERFACE_COLOR_foreground		0xFFF0F0F0

	#define	LIB_INTERFACE_NAME_limit		64

	#define	LIB_INTERFACE_SHADOW_length		8

	#define	LIB_INTERFACE_ELEMENT_LABEL_OR_BUTTON_NAME_limit	0xFFFF

	struct LIB_INTERFACE_STRUCTURE {
		struct STD_WINDOW_STRUCTURE_DESCRIPTOR	*descriptor;
		uint8_t		*properties;
		uint16_t	width;
		uint16_t	height;
		uint8_t		length;
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

	struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON {
		struct LIB_INTERFACE_STRUCTURE_ELEMENT	label_or_button;
		uint16_t	length;
		uint8_t		name[ EMPTY ];
	};

	// properties of Interface assigned to Window
	void lib_interface( struct LIB_INTERFACE_STRUCTURE *interface );

	// changes JSON format to internal Interface format
	void lib_interface_convert( struct LIB_INTERFACE_STRUCTURE *interface );

	// parse all elements of interface and show them
	void lib_interface_draw( struct LIB_INTERFACE_STRUCTURE *interface );

	// find element properties by its ID
	struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *lib_interface_element_by_id( struct LIB_INTERFACE_STRUCTURE *interface, uint64_t id );

	// show label element of definied properties
	void lib_interface_element_label( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *element );

	// rename window header
	void lib_interface_name( struct LIB_INTERFACE_STRUCTURE *interface );

	//
	void lib_interface_shadow( struct LIB_INTERFACE_STRUCTURE *interface );

	// create window space accoring to JSON specification
	void lib_interface_window( struct LIB_INTERFACE_STRUCTURE *interface );
#endif