/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_INTERFACE
	#define	LIB_INTERFACE

	#ifndef	LIB_JSON
		#include	"./json.h"
	#endif
	#ifndef	LIB_STRING
		#include	"./string.h"
	#endif

	#define	LIB_INTERFACE_HEADER_HEIGHT_pixel	(LIB_FONT_HEIGHT_pixel + 6)	// 3 pixels from above and under

	#define	LIB_INTERFACE_BORDER_pixel		1

	struct LIB_INTERFACE_STRUCTURE {
		uint8_t		*properties;
		uint16_t	width;
		uint16_t	height;
		struct STD_WINDOW_STRUCTURE_DESCRIPTOR	*descriptor;
	};

	// properties of Interface assigned to Window
	void lib_interface( struct LIB_INTERFACE_STRUCTURE *interface );

	// changes JSON format to internal Interface format
	void lib_interface_convert( struct LIB_INTERFACE_STRUCTURE *interface );

	// create window space accoring to JSON specification
	void lib_interface_window( struct LIB_INTERFACE_STRUCTURE *interface );
#endif