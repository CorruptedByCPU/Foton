/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/interface.h"
	#include	"../library/rgl.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"3d/data.c"
	//----------------------------------------------------------------------
	// routines, procedures
	//----------------------------------------------------------------------
	#include	"3d/config.h"
	#include	"3d/object.c"
	#include	"3d/init.c"

void close( void ) {
	// end of program
	exit();
}

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize environment
	init();

	//----------------------------------------------------------------------

	// parse object properties
	object_load( argc, argv );

	//----------------------------------------------------------------------

	// change camera position
	rgl -> camera.y = -1.0f;

	// array of parsed faces
	struct LIB_RGL_STRUCTURE_TRIANGLE *parse = (struct LIB_RGL_STRUCTURE_TRIANGLE *) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE ) * face_limit );

	// array of faces sorted by Z axis
	struct LIB_RGL_STRUCTURE_TRIANGLE **sort = (struct LIB_RGL_STRUCTURE_TRIANGLE **) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE **) * face_limit );

	// array of perspective
	struct LIB_RGL_STRUCTURE_MATRIX p_matrix = lib_rgl_return_matrix_perspective( rgl, 90.0f, (double) rgl -> width_pixel / (double) rgl -> height_pixel, 0.01f, 1.0f );

	// array of view
	struct LIB_RGL_STRUCTURE_MATRIX v_matrix = lib_rgl_return_matrix_view( rgl );

	// array of world
	struct LIB_RGL_STRUCTURE_MATRIX w_matrix = lib_rgl_multiply_matrix( v_matrix, p_matrix );

	// rotation angle
	double a = 0.0f;

	// alloc area for modified vectors
	vector3f *vector_transformed = (vector3f *) malloc( sizeof( vector3f ) * vector_limit );
	vector3f *vector_ready = (vector3f *) malloc( sizeof( vector3f ) * vector_limit );

	// main loop
	while( TRUE ) {
		// check events from interface
		if( ! the_master_of_puppets ) {
			// check incomming events
			struct LIB_INTERFACE_STRUCTURE *new = EMPTY;
			if( (new = lib_interface_event( d3_interface )) ) {
				// update interface pointer
				d3_interface = new;

				// reinitizalize RGL areas
				lib_rgl_resize( rgl, d3_interface -> width - (LIB_INTERFACE_BORDER_pixel << STD_SHIFT_2), d3_interface -> height - (LIB_INTERFACE_HEADER_HEIGHT_pixel + LIB_INTERFACE_BORDER_pixel), d3_interface -> width, (uint32_t *) ((uintptr_t) d3_interface -> descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ) + (((LIB_INTERFACE_HEADER_HEIGHT_pixel * d3_interface -> width) + LIB_INTERFACE_BORDER_pixel) << STD_VIDEO_DEPTH_shift)) );

				// update window content on screen
				d3_interface -> descriptor -> flags |= LIB_WINDOW_FLAG_resizable | LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;
			}
		}

		// recieve key
		uint16_t key = getkey();

		// exit game?
		if( key == STD_ASCII_ESC ) return 0;	// yes

		// next angle
		a += 0.02f;

		// clean workbench with default background color
		lib_rgl_clean( rgl );

		// calculate rotation matrixes
		// struct LIB_RGL_STRUCTURE_MATRIX x_matrix = lib_rgl_return_matrix_rotate_x( a / 2.0f );
		struct LIB_RGL_STRUCTURE_MATRIX y_matrix = lib_rgl_return_matrix_rotate_y( a );
		// struct LIB_RGL_STRUCTURE_MATRIX z_matrix = lib_rgl_return_matrix_rotate_z( a / 3.0f );

		// calculate translate matrix
		struct LIB_RGL_STRUCTURE_MATRIX t_matrix = lib_rgl_return_matrix_translate( 0.0f, 0.0f, 0.0f );

		// convert each vector
		for( uint64_t i = 1; i < vector_limit; i++ ) {
			// converted vector
			vector_transformed[ i ] = vector[ i ];

			// by Q matrix
			// lib_rgl_multiply_vector( &vector_transformed[ i ], &x_matrix );
			lib_rgl_multiply_vector( &vector_transformed[ i ], &y_matrix );
			// lib_rgl_multiply_vector( &vector_transformed[ i ], &z_matrix );
			lib_rgl_multiply_vector( &vector_transformed[ i ], &t_matrix );
		}

		// amount of faces to sort
		uint64_t sc = 0;

		// for each face
		for( uint64_t i = 1; i <= face_limit; i++ ) {
			// check face visibility
			if( lib_rgl_projection( rgl, vector_transformed, &face[ i ] ) ) {
				// face to parse
				parse[ i ] = face[ i ];

				// parse each vector of face
				for( uint8_t p = 0; p < 3; p++ ) {
					// vector number
					uint64_t v = parse[ i ].v[ p ];

					// parsed vector
					vector_ready[ v ] = vector_transformed[ v ];

					// by W matrix
					lib_rgl_multiply_vector( &vector_ready[ v ], &w_matrix );
				}

				// put parsed face into sorting list
				sort[ sc ] = &parse[ i ];

				// for sorting list, calculate average value of depth for 3 points of face
				sort[ sc ] -> z_depth = (vector_ready[ parse[ i ].v[ 0 ] ].z + vector_ready[ parse[ i ].v[ 1 ] ].z + vector_ready[ parse[ i ].v[ 2 ] ].z) / 3.0f;

				// sort list incremented
				sc++;
			}
		}

		// sort faces by depth
		if( sc > 1 ) lib_rgl_sort_quick( sort, sc - 1, 1 );

		// draw every triangle on workbench
		for( uint64_t i = 0; i < sc; i++ ) lib_rgl_fill( rgl, sort[ i ], vector_ready, material );

		// synchronize workbench with window
		lib_rgl_flush( rgl );

		// tell window manager to flush window
		if( ! the_master_of_puppets ) d3_interface -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;
	}

	// should not happen
	return 0;
}
