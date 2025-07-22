/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_KURO
	#define LIB_KURO

	#define	LIB_KURO_FLAG_size	0x01
	#define	LIB_KURO_FLAG_header	0x80

	enum LIB_KURO_MIMETYPE {
		LIB_KURO_MIMETYPE_UNKNOWN,
		LIB_KURO_MIMETYPE_UP,
		LIB_KURO_MIMETYPE_DIRECTORY,
		LIB_KURO_MIMETYPE_LINK,
		LIB_KURO_MIMETYPE_EXECUTABLE,
		LIB_KURO_MIMETYPE_LIBRARY,
		LIB_KURO_MIMETYPE_MODULE,
		LIB_KURO_MIMETYPE_IMAGE,
		LIB_KURO_MIMETYPE_PLAIN_TEXT,
		LIB_KURO_MIMETYPE_LOG,
		LIB_KURO_MIMETYPE_OBJECT,
		LIB_KURO_MIMETYPE_HEADER
	};

	struct LIB_KURO_STRUCTURE {
		struct LIB_UI_STRUCTURE_ELEMENT_TABLE_HEADER	*header;
		struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ENTRY	*entries;
		uint64_t					cols;
		uint64_t					rows;
		uint32_t					**icon;
		uint8_t						flag;
	};

	void lib_kuro( struct LIB_KURO_STRUCTURE *kuro );
#endif
