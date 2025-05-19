/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_WINDOW
	#include	"./window.h"
#endif

struct LIB_UI_STRUCTURE {
	struct LIB_WINDOW_STRUCTURE	*window;
};

struct LIB_UI_STRUCTURE *lib_ui( struct LIB_WINDOW_STRUCTURE *window );
