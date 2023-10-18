/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_JSON
	#define LIB_JSON

	#ifndef	LIB_STRING
		#include	"./string.h"
	#endif

	#define	LIB_JSON_TYPE_unknown	0
	#define	LIB_JSON_TYPE_string	1
	#define LIB_JSON_TYPE_number	2
	#define LIB_JSON_TYPE_object	3
	#define LIB_JSON_TYPE_array	4
	#define LIB_JSON_TYPE_boolean	5
	#define	LIB_JSON_TYPE_null	6

	struct LIB_JSON_STRUCTURE {
		uint8_t		type;
		uint8_t		*key;
		uint64_t	value;
		uint64_t	length;	// amount of characters in string or integer (depending on type)
		uint8_t		*next;
	};

	// return properties of first object of current JSON structure
	struct LIB_JSON_STRUCTURE lib_json( uint8_t *string );

	// compare key with selected string, return boolean
	uint8_t lib_json_key( struct LIB_JSON_STRUCTURE json, uint8_t *string );

	// select next object from JSON structure
	uint8_t lib_json_next( struct LIB_JSON_STRUCTURE *json );

	// retrieve properties of current object
	void lib_json_parse( struct LIB_JSON_STRUCTURE *json, uint8_t *string );

	// remove all white spaces, and return new length of json structure
	uint64_t lib_json_squeeze( uint8_t *json );
#endif