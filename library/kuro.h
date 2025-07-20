/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_KURO
	#define LIB_KURO

	enum LIB_KURO_MIMETYPE {
		UNKNOWN,
		UP,
		DIRECTORY,
		LINK,
		EXECUTABLE,
		LIBRARY,
		MODULE,
		IMAGE,
		PLAIN_TEXT,
		LOG,
		OBJECT,
		HEADER
	};

	struct LIB_KURO_STRUCTURE {
		struct LIB_UI_STRUCTURE_ELEMENT_TABLE_HEADER	*header;
		struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ROW	*row;
		uint64_t					count;
		uint32_t					**icon;
	};

	void lib_kuro( struct LIB_KURO_STRUCTURE *kuro );
#endif
