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

double distance( vector3f point, vector3f plane_n, vector3f plane_p ) {
	lib_rgl_vector_normalize( &point );

	double dot = lib_rgl_vector_product_dot( &plane_n, &plane_p );
	return (plane_n.x * point.x) + (plane_n.y * point.y) + (plane_n.z * point.z) - dot;
}

uint64_t clipper( struct LIB_RGL_STRUCTURE_TRIANGLE it, vector3f p, vector3f n, uint8_t side ) {
	// lib_terminal_printf( &terminal, (uint8_t *) "\n" );
	// if( vp[ it.v[ 0 ] ].x >= 0.0f ) lib_terminal_printf( &terminal, (uint8_t *) " " );
	// lib_terminal_printf( &terminal, (uint8_t *) "  %.2f", vp[ it.v[ 0 ] ].x );
	// if( vp[ it.v[ 0 ] ].y >= 0.0f ) lib_terminal_printf( &terminal, (uint8_t *) " " );
	// lib_terminal_printf( &terminal, (uint8_t *) "  %.2f", vp[ it.v[ 0 ] ].y );
	// if( vp[ it.v[ 0 ] ].z >= 0.0f ) lib_terminal_printf( &terminal, (uint8_t *) " " );
	// lib_terminal_printf( &terminal, (uint8_t *) "  %.2f\n", vp[ it.v[ 0 ] ].z );
	// if( vp[ it.v[ 1 ] ].x >= 0.0f ) lib_terminal_printf( &terminal, (uint8_t *) " " );
	// lib_terminal_printf( &terminal, (uint8_t *) "  %.2f", vp[ it.v[ 1 ] ].x );
	// if( vp[ it.v[ 1 ] ].y >= 0.0f ) lib_terminal_printf( &terminal, (uint8_t *) " " );
	// lib_terminal_printf( &terminal, (uint8_t *) "  %.2f", vp[ it.v[ 1 ] ].y );
	// if( vp[ it.v[ 1 ] ].z >= 0.0f ) lib_terminal_printf( &terminal, (uint8_t *) " " );
	// lib_terminal_printf( &terminal, (uint8_t *) "  %.2f\n", vp[ it.v[ 1 ] ].z );
	// if( vp[ it.v[ 2 ] ].x >= 0.0f ) lib_terminal_printf( &terminal, (uint8_t *) " " );
	// lib_terminal_printf( &terminal, (uint8_t *) "  %.2f", vp[ it.v[ 2 ] ].x );
	// if( vp[ it.v[ 2 ] ].y >= 0.0f ) lib_terminal_printf( &terminal, (uint8_t *) " " );
	// lib_terminal_printf( &terminal, (uint8_t *) "  %.2f", vp[ it.v[ 2 ] ].y );
	// if( vp[ it.v[ 2 ] ].z >= 0.0f ) lib_terminal_printf( &terminal, (uint8_t *) " " );
	// lib_terminal_printf( &terminal, (uint8_t *) "  %.2f\n", vp[ it.v[ 2 ] ].z );

	vector3f pi[ 3 ]; uint8_t pic = 0;
	vector3f po[ 3 ]; uint8_t poc = 0;

	vector3f point[ 3 ];
	point[ 0 ] = vp[ it.v[ 0 ] ];
	point[ 1 ] = vp[ it.v[ 1 ] ];
	point[ 2 ] = vp[ it.v[ 2 ] ];

	switch( side ) {
		case 0: { point[ 0 ].x += ((double) framebuffer.width_pixel / 2.0f); point[ 1 ].x += ((double) framebuffer.width_pixel / 2.0f); point[ 2 ].x += ((double) framebuffer.width_pixel / 2.0f); break; }
		case 1: { point[ 0 ].y += ((double) framebuffer.height_pixel / 2.0f); point[ 1 ].y += ((double) framebuffer.height_pixel / 2.0f); point[ 2 ].y += ((double) framebuffer.height_pixel / 2.0f); break; }
		case 2: { point[ 0 ].x -= ((double) framebuffer.width_pixel / 2.0f); point[ 1 ].x -= ((double) framebuffer.width_pixel / 2.0f); point[ 2 ].x -= ((double) framebuffer.width_pixel / 2.0f); break; }
		case 3: { point[ 0 ].y -= ((double) framebuffer.height_pixel / 2.0f); point[ 1 ].y -= ((double) framebuffer.height_pixel / 2.0f); point[ 2 ].y -= ((double) framebuffer.height_pixel / 2.0f); break; }
		default: {
			point[ 0 ] = vr[ it.v[ 0 ] ];
			point[ 1 ] = vr[ it.v[ 1 ] ];
			point[ 2 ] = vr[ it.v[ 2 ] ];
		}
	}

	double d0 = distance( point[ 0 ], n, p );
	double d1 = distance( point[ 1 ], n, p );
	double d2 = distance( point[ 2 ], n, p );
	// lib_terminal_printf( &terminal, (uint8_t *) "  d0 %.2f d1 %.2f d2 %.2f\n", d0, d1, d2 );

	if( d0 >= 0.0f ) pic++;
	else poc++;
	if( d1 >= 0.0f ) pic++;
	else poc++;
	if( d2 >= 0.0f ) pic++;
	else poc++;

	lib_terminal_printf( &terminal, (uint8_t *) "%u(in) %u(out)\n", pic, poc );

	return pic;
}

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

	// array of projection
	struct LIB_RGL_STRUCTURE_MATRIX p_matrix = lib_rgl_return_matrix_projection( rgl );

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
		angle += 0.1f;

		// calculate rotation matrixes
		struct LIB_RGL_STRUCTURE_MATRIX y_matrix = lib_rgl_return_matrix_rotate_y( angle );

		// calculate movement matrix
		struct LIB_RGL_STRUCTURE_MATRIX t_matrix = lib_rgl_return_matrix_translate( 0.0f, 0.0f, 5.0f );

		for( uint64_t i = 1; i < vc; i++ ) {
			vr[ i ] = vector[ i ];
			lib_rgl_multiply_vector( &vr[ i ], &t_matrix );
			lib_rgl_multiply_vector( &vr[ i ], &y_matrix );
		}

		// amount of faces to sort
		uint64_t sc = 0;

		for( uint64_t i = 1; i <= fc; i++ ) {
			// check face visibility
			if( lib_rgl_projection( rgl, vr, &face[ i ] ) ) {
				uint8_t s = 0;

				// near
				s = 4;
				lib_terminal_printf( &terminal, (uint8_t *) "n      " );
				vector3f pn = { 0.0f, 0.0f, 0.1f };
				vector3f nn = { 0.0f, 0.0f, 1.0f };
				uint8_t c = clipper( parse[ i ], pn, nn, s );
				if( c != 0 && c != 3 ) continue;

				// face to parse
				parse[ i ] = face[ i ];

				uint64_t v0 = parse[ i ].v[ 0 ];
				uint64_t v1 = parse[ i ].v[ 1 ];
				uint64_t v2 = parse[ i ].v[ 2 ];

				vp[ v0 ] = vr[ v0 ];
				vp[ v1 ] = vr[ v1 ];
				vp[ v2 ] = vr[ v2 ];

				lib_rgl_multiply_vector( &vp[ v0 ], &p_matrix );
				lib_rgl_multiply_vector( &vp[ v1 ], &p_matrix );
				lib_rgl_multiply_vector( &vp[ v2 ], &p_matrix );

				// left
				s = 0;
				lib_terminal_printf( &terminal, (uint8_t *) "left   " );
				vector3f pl = { 0.0f, 0.0f, 0.0f };
				vector3f nl = { 1.0f, 0.0f, 0.0f };
				clipper( parse[ i ], pl, nl, s );
				if( c != 0 && c != 3 ) continue;

				// top
				s = 1;
				lib_terminal_printf( &terminal, (uint8_t *) "top    " );
				vector3f pt = { 0.0f, 0.0f, 0.0f };
				vector3f nt = { 0.0f, 1.0f, 0.0f };
				clipper( parse[ i ], pt, nt, s );
				if( c != 0 && c != 3 ) continue;

				// right
				s = 2;
				lib_terminal_printf( &terminal, (uint8_t *) "right  " );
				vector3f pr = { 0.0f, 0.0f, 0.0f };
				vector3f nr = { -1.0f, 0.0f, 0.0f };
				clipper( parse[ i ], pr, nr, s );
				if( c != 0 && c != 3 ) continue;

				// bottom
				s = 3;
				lib_terminal_printf( &terminal, (uint8_t *) "bottom " );
				vector3f pb = { 0.0f, 0.0f, 0.0f };
				vector3f nb = { 0.0f, -1.0f, 0.0f };
				clipper( parse[ i ], pb, nb, s );
				if( c != 0 && c != 3 ) continue;

				sort[ sc ] = &parse[ i ];

				// calculate average value of depth for 3 points of face
				sort[ sc ] -> z_depth = (vp[ parse[ i ].v[ 0 ] ].z + vp[ parse[ i ].v[ 1 ] ].z + vp[ parse[ i ].v[ 2 ] ].z) / 3.0f;

				sc++;
			}
		}

		lib_terminal_printf( &terminal, (uint8_t *) "angle  %.2f", angle );

		// sort faces by Z axis
		if( sc > 1 ) lib_rgl_sort_quick( sort, sc - 1, 1 );

		// draw every triangle on workbench
		for( uint64_t i = 0; i < sc; i++ ) lib_rgl_triangle( rgl, sort[ i ], vp, material );

		// reset terminal cursor
		terminal.cursor_x = 0;
		terminal.cursor_y = 0;
		lib_terminal_cursor_set( &terminal );

		// lib_terminal_printf( &terminal, (uint8_t *) "\n" );
		// for( uint64_t i = 1; i < vc; i++ ) {
		// 	if( vr[ i ].x >= 0.0f ) lib_terminal_printf( &terminal, (uint8_t *) " " );
		// 	lib_terminal_printf( &terminal, (uint8_t *) "  %.2f", vr[ i ].x );
		// 	if( vr[ i ].y >= 0.0f ) lib_terminal_printf( &terminal, (uint8_t *) " " );
		// 	lib_terminal_printf( &terminal, (uint8_t *) "  %.2f", vr[ i ].y );
		// 	if( vr[ i ].z >= 0.0f ) lib_terminal_printf( &terminal, (uint8_t *) " " );
		// 	lib_terminal_printf( &terminal, (uint8_t *) "  %.2f\n", vr[ i ].z );
		// }

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
