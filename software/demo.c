/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/math.h"
	#include	"../library/rgl.h"
	#include	"../library/string.h"
	#include	"../library/terminal.h"

	#include	"demo/data.c"
	#include	"demo/object.c"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// obtain information about kernels framebuffer
	std_syscall_framebuffer( &framebuffer );

	// unsupported pitch size?
	if( framebuffer.pitch_byte >> STD_VIDEO_DEPTH_shift > framebuffer.width_pixel ) return -1;	// yes

	// initialize RGL library
	struct LIB_RGL_STRUCTURE *rgl = lib_rgl( framebuffer.width_pixel, framebuffer.height_pixel, framebuffer.base_address );

	//----------------------------------------------------------------------

	// update terminal properties
	terminal.width			= framebuffer.width_pixel;
	terminal.height			= framebuffer.height_pixel;
	terminal.base_address		= rgl -> workbench_base_address;
	terminal.scanline_pixel		= framebuffer.pitch_byte >> STD_VIDEO_DEPTH_shift;
	terminal.color_foreground	= STD_COLOR_WHITE;
	terminal.color_background	= STD_COLOR_BLACK;

	// initialize terminal
	lib_terminal( &terminal );

	// disable cursor
	lib_terminal_cursor_disable( &terminal );

	//----------------------------------------------------------------------

	object_load();

	//----------------------------------------------------------------------

	// array of parsed faces
	struct LIB_RGL_STRUCTURE_TRIANGLE *parse = (struct LIB_RGL_STRUCTURE_TRIANGLE *) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE ) * fc );

	// array of faces sorted by Z axis
	struct LIB_RGL_STRUCTURE_TRIANGLE **sort = (struct LIB_RGL_STRUCTURE_TRIANGLE **) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE **) * fc );

	// angle of rotation
	double angle = 0.0f;

	// array of perspective
	struct LIB_RGL_STRUCTURE_MATRIX p_matrix = lib_rgl_return_matrix_perspective( rgl, 0.78f, (double) rgl -> width_pixel / (double) rgl -> height_pixel, 0.01f, 1.0f );

	// array of view
	struct LIB_RGL_STRUCTURE_MATRIX v_matrix = lib_rgl_return_matrix_view( rgl );

	// array of world
	struct LIB_RGL_STRUCTURE_MATRIX w_matrix = lib_rgl_multiply_matrix( v_matrix, p_matrix );

	// calculate movement matrix
	double scale = 10.0f;
	struct LIB_RGL_STRUCTURE_MATRIX s_matrix = lib_rgl_return_matrix_translate( scale, scale, scale );

	// uint64_t last_uptime = 0;
	// uint64_t fps = 0;

	// main loop
	while( TRUE ) {
		// clean workbench with default background color
		lib_rgl_clean( rgl );

		// next angle
		angle += 0.15f;

		// calculate rotation matrixes
		struct LIB_RGL_STRUCTURE_MATRIX x_matrix = lib_rgl_return_matrix_rotate_x( angle );
		struct LIB_RGL_STRUCTURE_MATRIX y_matrix = lib_rgl_return_matrix_rotate_y( angle / 2.0f );
		struct LIB_RGL_STRUCTURE_MATRIX z_matrix = lib_rgl_return_matrix_rotate_z( angle / 3.0f );

		// calculate movement matrix
		struct LIB_RGL_STRUCTURE_MATRIX t_matrix = lib_rgl_return_matrix_translate( 0.0f, 0.0f, 0.0f );

		for( uint64_t i = 1; i < vc; i++ ) {
			vr[ i ] = vector[ i ];
			vr[ i ] = lib_rgl_multiply_vector( vr[ i ], x_matrix );
			vr[ i ] = lib_rgl_multiply_vector( vr[ i ], y_matrix );
			vr[ i ] = lib_rgl_multiply_vector( vr[ i ], z_matrix );
			vr[ i ] = lib_rgl_multiply_vector( vr[ i ], t_matrix );
		}

		// amount of faces to sort
		uint64_t sc = 0;

		for( uint64_t i = 1; i <= fc; i++ ) {
			// check face visibility
			if( lib_rgl_projection( rgl, vr, &face[ i ] ) ) {
				// face to parse
				parse[ i ] = face[ i ];
				
				for( uint8_t p = 0; p < 3; p++ ) {
					uint64_t v = parse[ i ].v[ p ];

					vp[ v ] = vr[ v ];

					vp[ v ] = lib_rgl_multiply_vector( vp[ v ], w_matrix );

				}

				sort[ sc ] = &parse[ i ];

				// calculate average value of depth for 3 points of face
				sort[ sc ] -> z_depth = (vp[ parse[ i ].v[ 0 ] ].z + vp[ parse[ i ].v[ 1 ] ].z + vp[ parse[ i ].v[ 2 ] ].z) / 3.0f;

				sc++;
			}
		}

		// sort faces by Z axis
		if( sc > 1 ) lib_rgl_sort_quick( sort, sc - 1, 1 );

		// draw every triangle on workbench
		for( uint64_t i = 0; i < sc; i++ ) lib_rgl_triangle( rgl, sort[ i ], vp, material );

		// reset terminal cursor
		terminal.cursor_x = 0;
		terminal.cursor_y = 0;
		lib_terminal_cursor_set( &terminal );

		// fps++;

		// uint64_t uptime = std_uptime();
		// if( uptime > last_uptime ) {
		// 	last_uptime += 1000;
		// 	lib_terminal_printf( &terminal, (uint8_t *) "\n  %u FPS (%u)", fps, uptime );
		// }

		// synchronize workbench with framebuffer
		lib_rgl_flush( rgl );
	}
}
