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

uint64_t lib_rgl_partition( struct LIB_RGL_STRUCTURE_TRIANGLE **triangles, uint64_t low, uint64_t high ) {
	double pivot = triangles[ high ] -> z_depth;

	uint64_t i = (low - 1);

	for( uint64_t j = low; j < high; j++ ) {
		if( triangles[ j ] -> z_depth < pivot ) {
			i++;

			struct LIB_RGL_STRUCTURE_TRIANGLE *tmp = triangles[ i ];
			triangles[ i ] = triangles[ j ];
			triangles[ j ] = tmp;
		}
	}

	struct LIB_RGL_STRUCTURE_TRIANGLE *tmp = triangles[ i + 1 ];
	triangles[ i + 1 ] = triangles[ high ];
	triangles[ high ] = tmp;

	return (i + 1);
}

void lib_rgl_sort_quick( struct LIB_RGL_STRUCTURE_TRIANGLE **triangles, uint64_t low, uint64_t high ) {
	if (low < high) {
		uint64_t pi = 0;
		pi = lib_rgl_partition( triangles, low, high );

		lib_rgl_sort_quick( triangles, low, pi - 1 );
		lib_rgl_sort_quick( triangles, pi + 1, high );
	}
}

inline uint8_t lib_rgl_edge( vector2d *a, vector2d *b, vector2d *c ) {
	if( (b -> x - a -> x) * (c -> y - a -> y) - (b -> y - a -> y) * (c -> x - a -> x) < 0 ) return TRUE;
	return FALSE;
}

void lib_rgl_line( struct LIB_RGL_STRUCTURE *rgl, int64_t x0, int64_t y0, int64_t x1, int64_t y1, uint32_t color ) {
	// no cliiping
	if( x0 < 0 ) x0 = 0;
	if( y0 < 0 ) y0 = 0;
	if( x1 < 0 ) x1 = 0;
	if( y1 < 0 ) y1 = 0;
	if( x0 >= rgl -> width_pixel ) x0 = rgl -> width_pixel - 1;
	if( y0 >= rgl -> height_pixel ) y0 = rgl -> height_pixel - 1;
	if( x1 >= rgl -> width_pixel ) x1 = rgl -> width_pixel - 1;
	if( y1 >= rgl -> height_pixel ) y1 = rgl -> height_pixel - 1;

	int64_t dx = abs( x1 - x0 ), sx = x0 < x1 ? 1 : -1;
	int64_t dy = -abs( y1 - y0 ), sy = y0 < y1 ? 1 : -1; 
	int64_t err = dx + dy, e2;

	for( ; ; ) {
		rgl -> workbench_base_address[ (y0 * rgl -> width_pixel) + x0 ] = color;

		if (x0 == x1 && y0 == y1) break;

		e2 = 2 * err;
	
		if (e2 >= dy) { err += dy; x0 += sx; }
		if (e2 <= dx) { err += dx; y0 += sy; }
	}
}

uint32_t lib_rgl_color( uint32_t argb, double light ) {
	double red = (double) ((argb & 0x00FF0000) >> 16);
	double green = (double) ((argb & 0x0000FF00) >> 8);
	double blue = (double) (argb & 0x000000FF);

	if ( light < 0.0f ) {
		light += 1.0f;
        
		red *= light;
        	green *= light;
		blue *= light;
    	} else {
	        red = (255.0f - red) * light + red;
	        green = (255.0f - green) * light + green;
	        blue = (255.0f - blue) * light + blue;
	}

	return STD_COLOR_mask | (((uint32_t) red) << 16) | (((uint32_t) green) << 8) | ((uint32_t) blue);
}

void lib_rgl_triangle( struct LIB_RGL_STRUCTURE *rgl, struct LIB_RGL_STRUCTURE_TRIANGLE *t, struct LIB_RGL_STRUCTURE_MATERIAL *material ) {
	uint32_t color = lib_rgl_color( material[ t -> material ].Kd, t -> light );

	vector2d p0 = { (int64_t) vp[ t -> v[ 0 ] ].x, (int64_t) vp[ t -> v[ 0 ] ].y };
	vector2d p1 = { (int64_t) vp[ t -> v[ 1 ] ].x, (int64_t) vp[ t -> v[ 1 ] ].y };
	vector2d p2 = { (int64_t) vp[ t -> v[ 2 ] ].x, (int64_t) vp[ t -> v[ 2 ] ].y };

	int64_t left = p0.x;
	if( p1.x < left ) left = p1.x;
	if( p2.x < left ) left = p2.x;

	int64_t top = p0.y;
	if( p1.y < top ) top = p1.y;
	if( p2.y < top ) top = p2.y;

	int64_t right = p0.x;
	if( p1.x > right ) right = p1.x;
	if( p2.x > right ) right = p2.x;

	int64_t bottom = p0.y;
	if( p1.y > bottom ) bottom = p1.y;
	if( p2.y > bottom ) bottom = p2.y;

	vector2d p;

	for( p.y = top; p.y <= bottom; p.y++ )
		for( p.x = left; p.x <= right; p.x++ ) {
			// pixel is inside workbench?
			int64_t x = p.x + (rgl -> width_pixel / 2);
			int64_t y = p.y + (rgl -> height_pixel / 2);
			if( x < 0 || x > rgl -> width_pixel ) continue;	// no
			if( y < 0 || y > rgl -> height_pixel ) continue;	// no

			// pixel inside triangle?
			if( ! lib_rgl_edge( (vector2d *) &p1, (vector2d *) &p2, (vector2d *) &p ) ) continue;
			if( ! lib_rgl_edge( (vector2d *) &p2, (vector2d *) &p0, (vector2d *) &p ) ) continue;
			if( ! lib_rgl_edge( (vector2d *) &p0, (vector2d *) &p1, (vector2d *) &p ) ) continue;

			// draw it
			rgl -> workbench_base_address[ (y * rgl -> width_pixel) + x ] = color;
		}

	lib_rgl_line( rgl, p0.x + (rgl -> width_pixel / 2), p0.y + (rgl -> height_pixel / 2), p1.x + (rgl -> width_pixel / 2), p1.y + (rgl -> height_pixel / 2), color );
	lib_rgl_line( rgl, p1.x + (rgl -> width_pixel / 2), p1.y + (rgl -> height_pixel / 2), p2.x + (rgl -> width_pixel / 2), p2.y + (rgl -> height_pixel / 2), color );
	lib_rgl_line( rgl, p2.x + (rgl -> width_pixel / 2), p2.y + (rgl -> height_pixel / 2), p0.x + (rgl -> width_pixel / 2), p0.y + (rgl -> height_pixel / 2), color );
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
	struct LIB_RGL_STRUCTURE_MATRIX p_matrix = lib_rgl_return_matrix_projection( rgl, 90.0f );

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
		angle += 0.10f;

		// calculate rotation matrixes
		struct LIB_RGL_STRUCTURE_MATRIX y_matrix = lib_rgl_return_matrix_rotate_y( angle );

		// calculate movement matrix
		struct LIB_RGL_STRUCTURE_MATRIX t_matrix = lib_rgl_return_matrix_translate( 0.0f, 0.0f, 4.0f );

		for( uint64_t i = 1; i < vc; i++ ) {
			vr[ i ] = vector[ i ];
			lib_rgl_multiply_vector( &vr[ i ], &y_matrix );
			lib_rgl_multiply_vector( &vr[ i ], &t_matrix );
		}

		// amount of faces to sort
		uint64_t sc = 0;

		for( uint64_t i = 1; i <= fc; i++ ) {
			// check face visibility
			if( lib_rgl_projection( rgl, vr, &face[ i ] ) ) {
				uint64_t v0 = parse[ i ].v[ 0 ];
				uint64_t v1 = parse[ i ].v[ 1 ];
				uint64_t v2 = parse[ i ].v[ 2 ];
				vp[ v0 ] = vr[ v0 ];
				vp[ v1 ] = vr[ v1 ];
				vp[ v2 ] = vr[ v2 ];
				lib_rgl_multiply_vector( &vp[ v0 ], &p_matrix );
				lib_rgl_multiply_vector( &vp[ v1 ], &p_matrix );
				lib_rgl_multiply_vector( &vp[ v2 ], &p_matrix );

				// face to parse
				parse[ i ] = face[ i ];

				// if yes, add to sorting list
				sort[ sc ] = &parse[ i ];

				// calculate average value of depth for 3 points of face
				sort[ sc ] -> z_depth = (vp[ parse[ i ].v[ 0 ] ].z + vp[ parse[ i ].v[ 1 ] ].z + vp[ parse[ i ].v[ 2 ] ].z) / 3.0f;

				uint64_t clip = 0;
				struct LIB_RGL_STRUCTURE_TRIANGLE t[ 2 ];

				sc++;
			}
		}

		// sort faces by Z axis
		if( sc ) lib_rgl_sort_quick( sort, sc - 1, 1 );

		// draw every triangle on workbench
		for( uint64_t i = 0; i < sc; i++ ) lib_rgl_triangle( rgl, sort[ i ], material );

		// reset terminal cursor
		terminal.cursor_x = 0;
		terminal.cursor_y = 0;
		lib_terminal_cursor_set( &terminal );

		lib_terminal_printf( &terminal, (uint8_t *) "\n" );
		for( uint64_t i = 1; i < vc; i++ ) {
			if( vp[ i ].x >= 0.0f ) lib_terminal_printf( &terminal, (uint8_t *) " " );
			lib_terminal_printf( &terminal, (uint8_t *) "  %.2f", vp[ i ].x );
			if( vp[ i ].y >= 0.0f ) lib_terminal_printf( &terminal, (uint8_t *) " " );
			lib_terminal_printf( &terminal, (uint8_t *) "  %.2f", vp[ i ].y );
			if( vp[ i ].z >= 0.0f ) lib_terminal_printf( &terminal, (uint8_t *) " " );
			lib_terminal_printf( &terminal, (uint8_t *) "  %.2f\n", vp[ i ].z );
		}

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
