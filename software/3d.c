/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/font.h"
	#include	"../library/rgl.h"
	#include	"../library/ui.h"
	//----------------------------------------------------------------------
	// static, structures, definitions
	//----------------------------------------------------------------------
	#include	"3d/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"3d/data.c"
	//----------------------------------------------------------------------
	// routines, procedures
	//----------------------------------------------------------------------
	#include	"3d/object.c"
	#include	"3d/init.c"
	//----------------------------------------------------------------------

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

	// alloc area for modified vectors
	vector3f *vector_transformed = (vector3f *) malloc( sizeof( vector3f ) * vector_limit );
	vector3f *vector_ready = (vector3f *) malloc( sizeof( vector3f ) * vector_limit );

	// speed of rotation
	double speed = 0.128f;
	double time = 0.10f;	// 1/60 of second

	// current angle
	double a = 0.0f;

	// start measuring
	microtime = std_microtime();

	// main loop
	while( TRUE ) {
		// check for
		lib_ui_event( ui );

		// recieve key
		uint16_t key = getkey();

		// exit?
		if( key == STD_ASCII_ESC ) return 0;	// yes

		// changed window properties?
		if( window != ui -> window ) {
			// update
			window = ui -> window;
			
			// reinitialize RGL library
			rgl = lib_rgl( ui -> window -> current_width - (LIB_UI_BORDER_DEFAULT << STD_SHIFT_2), ui -> window -> current_height - (LIB_UI_HEADER_HEIGHT + LIB_UI_BORDER_DEFAULT), ui -> window -> current_width, (uint32_t *) ui -> window -> pixel + (LIB_UI_HEADER_HEIGHT * ui -> window -> current_width) + LIB_UI_BORDER_DEFAULT );

			ui -> window -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;
		}

		// next angle
		a = ((double) std_microtime() / 360.0f) * 5.0f;
		// a += speed * time;
		if( a > 360.0f ) a-= 360.0f;

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

		// frame ready
		fps++;

		// show FPS
		uint32_t *pixel = (uint32_t *) ui -> window -> pixel + (LIB_UI_HEADER_HEIGHT * ui -> window -> current_width) + LIB_UI_BORDER_DEFAULT;
		pixel += 4 + (ui -> window -> current_width * 4);
		lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "FPS: ", 5, STD_COLOR_WHITE, pixel, ui -> window -> current_width, EMPTY );
		pixel += LIB_FONT_FAMILY_ROBOTO_MONO_KERNING * 5;
		lib_font_value( LIB_FONT_FAMILY_ROBOTO_MONO, fps_show, 10, STD_COLOR_WHITE, pixel, ui -> window -> current_width, EMPTY );
		pixel += -(LIB_FONT_FAMILY_ROBOTO_MONO_KERNING * 5) + ui -> window -> current_width * LIB_FONT_HEIGHT_pixel;
		lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "Avg: ~", 6, STD_COLOR_WHITE, pixel, ui -> window -> current_width, EMPTY );
		pixel += LIB_FONT_FAMILY_ROBOTO_MONO_KERNING * 6;
		if( fps_avarage_c ) lib_font_value( LIB_FONT_FAMILY_ROBOTO_MONO, fps_avarage / fps_avarage_c, 10, STD_COLOR_WHITE, pixel, ui -> window -> current_width, EMPTY );

		// 1 second passed?
		if( std_microtime() > microtime + 1024 ) {
			// restart measurements variables
			fps_show = fps;
			fps_avarage += fps;
			fps_avarage_c++;
			fps = EMPTY;

			// start new measurement
			microtime = std_microtime();
		}

		// tell window manager to flush window
		ui -> window -> flags |= LIB_WINDOW_FLAG_flush;

// debug
// while( TRUE );
	}

	// should not happen
	return 0;
}
