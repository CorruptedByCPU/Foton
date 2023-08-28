
/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/font.h"
	#include	"../library/math.h"
	#include	"../library/rgl.h"
	#include	"../library/string.h"
	#include	"../library/terminal.h"

	#include	"demo/data.c"
	#include	"demo/object.c"
	#include	"demo/init.c"

void interface( void ) {
	// sad hack :|
	__asm__ volatile( "testw $0x08, %sp\nje .+4\npushq $0x00" );

	// terminal properties
	struct LIB_TERMINAL_STRUCTURE terminal;

	// update terminal properties
	terminal.width			= framebuffer.width_pixel;
	terminal.height			= framebuffer.height_pixel;
	terminal.base_address		= framebuffer.base_address;
	terminal.scanline_pixel		= framebuffer.pitch_byte >> STD_VIDEO_DEPTH_shift;
	terminal.color_foreground	= STD_COLOR_WHITE;
	terminal.color_background	= STD_COLOR_BLACK;

	// initialize terminal
	lib_terminal( &terminal );

	// disable cursor
	lib_terminal_cursor_disable( &terminal );

	while( TRUE ) {
		// unit of time, passed?
		if( std_uptime() > fpu_last ) {
			// select new time unit
			fpu_last += 10;

			// sum all FPU parsed
			fpu_average += fpu;
			fpu_count++;

			// reset terminal cursor
			terminal.cursor_x = 0;
			terminal.cursor_y = 0;
			lib_terminal_cursor_set( &terminal );

			// show amount of FPU
			lib_terminal_printf( &terminal, (uint8_t *) "FPu: %u (%u) ", fpu_average / fpu_count, fpu );

			// restart
			fpu = EMPTY;
		}
	}
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize environment
	if( init() ) return -1;

	// execute interface as separate thread
	std_syscall_thread( (uintptr_t) &interface, (uint8_t *) "demo-interface", 14 );

	//----------------------------------------------------------------------

	// array of parsed faces
	struct LIB_RGL_STRUCTURE_TRIANGLE *parse = (struct LIB_RGL_STRUCTURE_TRIANGLE *) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE ) * fc );

	// array of faces sorted by Z axis
	struct LIB_RGL_STRUCTURE_TRIANGLE **sort = (struct LIB_RGL_STRUCTURE_TRIANGLE **) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE **) * fc );

	// array of perspective
	struct LIB_RGL_STRUCTURE_MATRIX p_matrix = lib_rgl_return_matrix_perspective( rgl, 0.78f, (double) rgl -> width_pixel / (double) rgl -> height_pixel, 0.01f, 1.0f );

	// array of view
	struct LIB_RGL_STRUCTURE_MATRIX v_matrix = lib_rgl_return_matrix_view( rgl );

	// array of world
	struct LIB_RGL_STRUCTURE_MATRIX w_matrix = lib_rgl_multiply_matrix( v_matrix, p_matrix );

	// rotation angle
	double a = 0.0f;

	// main loop
	while( TRUE ) {
		// next angle
		a += 0.15f;

		// clean workbench with default background color
		lib_rgl_clean( rgl );

		// calculate rotation matrixes
		struct LIB_RGL_STRUCTURE_MATRIX x_matrix = lib_rgl_return_matrix_rotate_x( a );
		struct LIB_RGL_STRUCTURE_MATRIX y_matrix = lib_rgl_return_matrix_rotate_y( a / 2.0f );
		struct LIB_RGL_STRUCTURE_MATRIX z_matrix = lib_rgl_return_matrix_rotate_z( a / 3.0f );

		// calculate movement matrix
		struct LIB_RGL_STRUCTURE_MATRIX t_matrix = lib_rgl_return_matrix_translate( 0.0f, 0.0f, 0.0f );

		// convert each vector
		for( uint64_t i = 1; i < vc; i++ ) {
			// converted vector
			vr[ i ] = vector[ i ];

			// by Q matrix
			lib_rgl_multiply_vector( &vr[ i ], &x_matrix );
			lib_rgl_multiply_vector( &vr[ i ], &y_matrix );
			lib_rgl_multiply_vector( &vr[ i ], &z_matrix );
			lib_rgl_multiply_vector( &vr[ i ], &t_matrix );
		}

		// amount of faces to sort
		uint64_t sc = 0;

		// for each face
		for( uint64_t i = 1; i <= fc; i++ ) {
			// check face visibility
			if( lib_rgl_projection( rgl, vr, &face[ i ] ) ) {
				// face to parse
				parse[ i ] = face[ i ];

				// parse each vector of face
				for( uint8_t p = 0; p < 3; p++ ) {
					// vector number
					uint64_t v = parse[ i ].v[ p ];

					// parsed vector
					vp[ v ] = vr[ v ];

					// by W matrix
					lib_rgl_multiply_vector( &vp[ v ], &w_matrix );
				}

				// put parsed face into sorting list
				sort[ sc ] = &parse[ i ];

				// for sorting list, calculate average value of depth for 3 points of face
				sort[ sc ] -> z_depth = (vp[ parse[ i ].v[ 0 ] ].z + vp[ parse[ i ].v[ 1 ] ].z + vp[ parse[ i ].v[ 2 ] ].z) / 3.0f;

				// sort list incremented
				sc++;
			}
		}

		// sort faces by depth
		if( sc > 1 ) lib_rgl_sort_quick( sort, sc - 1, 1 );

		// draw every triangle on workbench
		for( uint64_t i = 0; i < sc; i++ ) lib_rgl_triangle( rgl, sort[ i ], vp, material );

		// synchronize workbench with framebuffer
		lib_rgl_flush( rgl );

		// next frame ready
		fpu++;
	}

	// should not happen
	return 0;
}