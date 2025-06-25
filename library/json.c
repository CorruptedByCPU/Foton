/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_JSON
		#include	"./json.h"
	#endif

struct LIB_JSON_STRUCTURE lib_json( uint8_t *string ) {
	// properties of JSON structure
	struct LIB_JSON_STRUCTURE json = { EMPTY };

	// object?
	if( *string != '{' ) return json;	// incorrect JSON structure

	// key
	json.key = ++string;

	// set other properties
	lib_json_parse( (struct LIB_JSON_STRUCTURE *) &json, string );

	// return JSON structure
	return json;
}

uint8_t lib_json_key( struct LIB_JSON_STRUCTURE json, uint8_t *string ) {
	// key name
	uint8_t *key = json.key + 1;

	// length of key name
	uint64_t length = lib_string_length( string );

	// check
	while( *key != '"' ) if( *(key++) == *(string++) ) length--; else return FALSE;

	// key recognized?
	if( ! length && *key == '"' ) return TRUE;	// yes

	// no
	return FALSE;
}

uint8_t lib_json_next( struct LIB_JSON_STRUCTURE *json ) {
	// next object exist?
	if( ! json -> next ) return FALSE;	// no

	// key
	json -> key = json -> next;

	// set other properties
	lib_json_parse( json, json -> key );

	// done
	return TRUE;
}

void lib_json_parse( struct LIB_JSON_STRUCTURE *json, uint8_t *string ) {
	// value
	while( *(++string) != '"' || *(string++ - 1) == '\\' );
	json -> value = (uint64_t) ++string;

	// initial length of next value
	json -> length = EMPTY;

	// search for next key
	switch( *string ) {
		case '{': {
			uint64_t i = TRUE;
			while( i ) {
				json -> length++;
				if( string[ json -> length ] == '{' ) i++;
				if( string[ json -> length ] == '}' ) i--;
			}

			json -> type = LIB_JSON_TYPE_object;
			if( string[ ++json -> length ] == '}' ) json -> next = EMPTY;
			else json -> next = &string[ json -> length + 1 ];

			break;
		}
		case '"': {
			while( string[ ++json -> length ] != '"' || string[ json -> length - 1 ] == '\\' );

			json -> type = LIB_JSON_TYPE_string;
			if( string[ ++json -> length ] == '}' ) json -> next = EMPTY;
			else json -> next = &string[ json -> length + 1 ];

			json -> value++;
			json -> length -= 2;

			break;
		}
		default: {
			if( *string == 'n' || *string == 'N' ) json -> type = LIB_JSON_TYPE_null;
			else if( *string == 't' || *string == 'T' || *string == 'f' || *string == 'F' ) { json -> type = LIB_JSON_TYPE_boolean; if( *string == 't' || *string == 'T' ) json -> value = TRUE; else json -> value = FALSE; }
			else if( *string == '-' || *string == '0' || (*string - STD_ASCII_DIGIT_0 < 10) ) { json -> type = LIB_JSON_TYPE_number; json -> value = lib_string_to_integer( (uint8_t *) json -> value, 10 ); }
			else json -> type = LIB_JSON_TYPE_unknown;

			json -> next = EMPTY;
			while( *string && *string != ',' ) if( *(string++) == '}' ) return;
			json -> next = ++string;
		}
	}
}

uint64_t lib_json_squeeze( uint8_t *json ) {
	// pointer to formatted json
	uint8_t *formatted = json;
	uint64_t i = EMPTY;	// formatted index

	// calculate new length
	do {
		// start of quote?
		if( *json == STD_ASCII_QUOTATION ) {
			// keep previous value
			uint8_t previous = *(json - 1);

			// count towards
			*(formatted++) = *(json++); i++;

			// look for end of quote
			while( *json != STD_ASCII_QUOTATION || previous == STD_ASCII_BACKSLASH ) {
				// update previous value
				previous = *json;
				
				// count towards
				*(formatted++) = *(json++); i++;
			}

			// count towards
			*(formatted++) = *json; i++;
		} else
			// character visible?
			if( *json > STD_ASCII_SPACE && *json < STD_ASCII_DELETE ) { *(formatted++) = *json; i++; }
	} while( *(json++) );

	// end of formatted json
	*formatted = STD_ASCII_TERMINATOR;

	// return new json length
	return i;
}
