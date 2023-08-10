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

	// unsupported pitch size?
	if( framebuffer.pitch_byte >> STD_VIDEO_DEPTH_shift > framebuffer.width_pixel ) return -1;	// yes

	// initialize RGL library
	struct LIB_RGL_STRUCTURE *rgl = lib_rgl( framebuffer.width_pixel, framebuffer.height_pixel, framebuffer.base_address );

	//----------------------------------------------------------------------

	// // amount of vectors and faces inside object file
	// uint64_t v, vc, f, fc = 0;

	// // count of vectors and faces
	// uint8_t *line = (uint8_t *) &file_object_start;
	// while( lib_string_length_line( line ) ) {
	// 	// vector?
	// 	if( line[ 0 ] == 'v' ) vc++;

	// 	// face?
	// 	if( line[ 0 ] == 'f' ) fc++;

	// 	// next line from file
	// 	line += lib_string_length_line( line ) + 1;	// omit line feed character
	// }

	// // alloc area for points and faces
	// vector3f *vector = (vector3f *) malloc( sizeof( vector3f ) * vc );
	// struct LIB_RGL_STRUCTURE_TRIANGLE *face = (struct LIB_RGL_STRUCTURE_TRIANGLE *) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE ) * fc );

	// // calculate amount of vectors and faces
	// line = (uint8_t *) &file_object_start;
	// while( line < (uint8_t *) &file_object_end ) {
	// 	// vector?
	// 	if( *line == 'v' ) {
	// 		// set point at first value
	// 		uint8_t *vs = (uint8_t *) &line[ 2 ];
	// 		uint64_t vl = lib_string_word( vs, lib_string_length_line( line ) );

	// 		// X axis
	// 		vector[ v ].x = strtof( vs, vl );

	// 		// set pointer at second value
	// 		vs += vl + 1;
	// 		vl = lib_string_word( vs, lib_string_length_line( line ) );

	// 		// Y axis
	// 		vector[ v ].y = strtof( vs, vl );

	// 		// set pointer at third value
	// 		vs += vl + 1;
	// 		vl = lib_string_word( vs, lib_string_length_line( line ) );

	// 		// Z axis
	// 		vector[ v ].z = strtof( vs, vl );

	// 		// W as default
	// 		vector[ v ].w = 1.0f;

	// 		// vector registered
	// 		v++;
	// 	}

	// 	// face?
	// 	if( *line == 'f' ) {
	// 		// set pointer at first value
	// 		uint8_t *fs = (uint8_t *) &line[ 2 ];
	// 		uint64_t fl = lib_string_length_scope_digit( fs );

	// 		// first point of face
	// 		face[ f ].point[ 0 ] = vector[ (uint64_t) lib_string_to_integer( fs, 10 ) ];

	// 		// set pointer at second value
	// 		fs += fl + 1;
	// 		fl = lib_string_length_scope_digit( fs );

	// 		// second point of face
	// 		face[ f ].point[ 1 ] = vector[ (uint64_t) lib_string_to_integer( fs, 10 ) ];

	// 		// set pointer at third value
	// 		fs += fl + 1;
	// 		fl = lib_string_length_scope_digit( fs );

	// 		// third point of face
	// 		face[ f ].point[ 2 ] = vector[ (uint64_t) lib_string_to_integer( fs, 10 ) ];

	// 		// face registered
	// 		f++;
	// 	}

	// 	// next line from file
	// 	line += lib_string_length_line( line ) + 1;	// omit line feed character
	// }

	// // we don't need a list of vectors anymore
	// free( vector );

	//----------------------------------------------------------------------

	// faces
	// struct LIB_RGL_STRUCTURE_TRIANGLE *parse = (struct LIB_RGL_STRUCTURE_TRIANGLE *) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE ) * fc );

	// main loop
	while( TRUE ) {
		// clean workbench with default background color
		// lib_rgl_clean( rgl );
	}
}
