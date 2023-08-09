/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/rgl.h"
	#include	"../library/string.h"

struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER framebuffer;

MACRO_IMPORT_FILE_AS_ARRAY( object, "./root/system/var/teapot.obj" );

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// obtain information about kernels framebuffer
	std_syscall_framebuffer( &framebuffer );

	//----------------------------------------------------------------------

	// amount of vectors and faces inside object file
	uint64_t v = 0;
	uint64_t f = 0;

	// alloc area for points and faces
	vector3f *vector = (vector3f *) malloc( sizeof( vector3f ) * v );
	struct LIB_RGL_STRUCTURE_TRIANGLE *face = (struct LIB_RGL_STRUCTURE_TRIANGLE *) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE ) * f );

	// calculate amount of vectors and faces
	uint8_t *line = (uint8_t *) &file_object_start;
	while( line < (uint8_t *) &file_object_end ) {
		// vector?
		if( *line == 'v' ) {
			// prepare area for point
			vector = (vector3f *) realloc( vector, sizeof( vector3f ) * v );

			// set point at first value
			uint8_t *vs = (uint8_t *) &line[ 2 ];
			uint64_t vl = lib_string_word( vs, lib_string_length_line( line ) );

			// X axis
			vector[ v ].x = strtof( vs, vl );

			// set pointer at second value
			vs += vl + 1;
			vl = lib_string_word( vs, lib_string_length_line( line ) );

			// Y axis
			vector[ v ].y = strtof( vs, vl );

			// set pointer at third value
			vs += vl + 1;
			vl = lib_string_word( vs, lib_string_length_line( line ) );

			// Z axis
			vector[ v ].z = strtof( vs, vl );

			// W as default
			vector[ v ].w = 1.0f;

			// vector registered
			v++;
		}

		// face?
		if( *line == 'f' ) {
			// new face to register
			f++;
		}

		// next line from file
		line += lib_string_length_line( line ) + 1;	// omit line feed character
	}

	//----------------------------------------------------------------------

	// hold the door
	while( TRUE );
}
