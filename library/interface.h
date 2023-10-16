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

	struct LIB_INTERFACE_STRUCTURE {
		uint8_t		*properties;
		uint16_t	width;
		uint16_t	height;
	};

	// properties of Interface assigned to Window
	void lib_interface( struct LIB_INTERFACE_STRUCTURE *interface );

	// changes JSON format to internal Interface format
	void lib_interface_convert( struct LIB_INTERFACE_STRUCTURE *interface );
#endif