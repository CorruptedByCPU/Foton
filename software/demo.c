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

struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER framebuffer;

struct LIB_TERMINAL_STRUCTURE terminal;

// MACRO_IMPORT_FILE_AS_ARRAY( object, "./root/system/var/test.obj" );
// MACRO_IMPORT_FILE_AS_ARRAY( object, "./root/system/var/teapot.obj" );
MACRO_IMPORT_FILE_AS_ARRAY( object, "./root/system/var/earth.obj" );
MACRO_IMPORT_FILE_AS_ARRAY( material, "./root/system/var/earth.mtl" );

uint8_t string_material[ 6 ] = "newmtl";
uint8_t string_material_change[ 6 ] = "usemtl";
uint8_t	string_kd[ 2 ] = "Kd";

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

	// amount of materials
	uint64_t m = 1;
	uint64_t mc = 1;

	// // count materials
	uint8_t *line = (uint8_t *) &file_material_start;
	while( line < (uint8_t *) &file_material_end ) {
		// material definition?
		if( lib_string_compare( line, (uint8_t *) &string_material, sizeof( string_material ) ) ) mc++;

		// next line from file
		line += lib_string_length_line( line ) + 1;	// omit line feed character
	}

	// alloc area for materials
	struct LIB_RGL_STRUCTURE_MATERIAL *material = (struct LIB_RGL_STRUCTURE_MATERIAL *) malloc( sizeof( struct LIB_RGL_STRUCTURE_MATERIAL ) * mc );

	// register materials
	line = (uint8_t *) &file_material_start;
	while( line < (uint8_t *) &file_material_end ) {
		// material definition?
		if( lib_string_compare( line, (uint8_t *) &string_material, sizeof( string_material ) ) ) {
			// set string pointer to material name
			line += sizeof( string_material ) + 1;

			// retrieve material name and length
			material[ m ].length = lib_string_length_line( line );
			for( uint64_t i = 0; i < material[ m ].length; i++ ) material[ m ].name[ i ] = line[ i ];

			// parse until end of material properties
			while( lib_string_length_line( line ) ) {
				// diffusion?
				if( lib_string_compare( line, (uint8_t *) &string_kd, sizeof( string_kd ) ) ) {
					// initialize color value
					material[ m ].Kd = STD_COLOR_mask;

					// set string pointer to first value
					uint8_t *p = (uint8_t *) line + sizeof( string_kd ) + 1;
					uint64_t pl = lib_string_word( p, lib_string_length_line( line ) );

					// Red
					material[ m ].Kd |= ((uint8_t) (255.0 * strtof( p, pl ))) << 16;

					// set pointer at second value
					p += pl + 1;
					pl = lib_string_word( p, lib_string_length_line( line ) );

					// Green
					material[ m ].Kd |= ((uint8_t) (255.0 * strtof( p, pl ))) << 8;

					// set pointer at third value
					p += pl + 1;
					pl = lib_string_word( p, lib_string_length_line( line ) );

					// Blue
					material[ m ].Kd |= (uint8_t) (255.0 * strtof( p, pl ));
				}

				// next line from file
				line += lib_string_length_line( line ) + 1;	// omit line feed character
			}

			// material registered
			m++;
		}

		// next line from file
		line += lib_string_length_line( line ) + 1;	// omit line feed character
	}

	// amount of vectors and faces inside object file
	uint64_t v = 1;
	uint64_t vc = 1;
	uint64_t f = 1;
	uint64_t fc = 1;

	// count vectors and faces
	line = (uint8_t *) &file_object_start;
	while( line < (uint8_t *) &file_object_end ) {
		// vector?
		if( line[ 0 ] == 'v' ) vc++;

		// face?
		if( line[ 0 ] == 'f' ) fc++;

		// next line from file
		line += lib_string_length_line( line ) + 1;	// omit line feed character
	}

	// alloc area for points and faces
	vector3f *vector = (vector3f *) malloc( sizeof( vector3f ) * vc );
	struct LIB_RGL_STRUCTURE_TRIANGLE *face = (struct LIB_RGL_STRUCTURE_TRIANGLE *) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE ) * fc );

	// register default material properties
	material[ 0 ].Kd = STD_COLOR_WHITE;

	// by default use initial material
	uint64_t material_id = 0;

	// register vectors and faces
	line = (uint8_t *) &file_object_start;
	while( line < (uint8_t *) &file_object_end ) {
		// vector?
		if( *line == 'v' ) {
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

		// change material?
		if( lib_string_compare( line, (uint8_t *) string_material_change, sizeof( string_material_change ) ) ) {
			// set pointer to material name
			line += sizeof( string_material_change ) + 1;

			// retrieve material id
			for( uint64_t i = 0; i < mc; i++ )
				// material name found?
				if( lib_string_compare( line, (uint8_t *) &material[ i ].name, lib_string_length_line( line ) ) )
					// yes
					material_id = i;
		}

		// face?
		if( *line == 'f' ) {
			// set pointer at first value
			uint8_t *fs = (uint8_t *) &line[ 2 ];
			uint64_t fl = lib_string_length_scope_digit( fs );

			// first point of face
			face[ f ].point[ 0 ] = vector[ (uint64_t) lib_string_to_integer( fs, 10 ) ];

			// set pointer at second value
			fs += fl + 1;
			fl = lib_string_length_scope_digit( fs );

			// second point of face
			face[ f ].point[ 1 ] = vector[ (uint64_t) lib_string_to_integer( fs, 10 ) ];

			// set pointer at third value
			fs += fl + 1;
			fl = lib_string_length_scope_digit( fs );

			// third point of face
			face[ f ].point[ 2 ] = vector[ (uint64_t) lib_string_to_integer( fs, 10 ) ];

			// use definied material
			face[ f ].material = material_id;

			// face registered
			f++;
		}

		// next line from file
		line += lib_string_length_line( line ) + 1;	// omit line feed character
	}

	// we don't need a list of vectors anymore
	free( vector );

	//----------------------------------------------------------------------

	// array of parsed faces
	struct LIB_RGL_STRUCTURE_TRIANGLE *parse = (struct LIB_RGL_STRUCTURE_TRIANGLE *) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE ) * fc );

	// array of faces sorted by Z axis
	struct LIB_RGL_STRUCTURE_TRIANGLE **sort = (struct LIB_RGL_STRUCTURE_TRIANGLE **) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE **) * fc );

	// angle of rotation
	double angle = 0.0f;

	// properties of projection
	double near = 0.0f;
	double far = 100.0f;
	double hfov = 90.0f;
	double vfov = hfov * ((double) rgl -> height_pixel / (double) rgl -> width_pixel);
	double aspect = (double) rgl -> width_pixel / (double) rgl -> height_pixel;

	// array of projection
	struct LIB_RGL_STRUCTURE_MATRIX p_matrix = lib_rgl_return_matrix_projection( near, far, hfov, aspect );

	// main loop
	while( TRUE ) {
		// clean workbench with default background color
		lib_rgl_clean( rgl );

		// next angle
		angle += 0.10f;

		// calculate rotation matrixes
		struct LIB_RGL_STRUCTURE_MATRIX z_matrix = lib_rgl_return_matrix_rotate_z( angle / 2.0f );
		struct LIB_RGL_STRUCTURE_MATRIX x_matrix = lib_rgl_return_matrix_rotate_x( 0.0f );
		struct LIB_RGL_STRUCTURE_MATRIX y_matrix = lib_rgl_return_matrix_rotate_y( angle );

		// calculate movement matrix
		struct LIB_RGL_STRUCTURE_MATRIX t_matrix = lib_rgl_return_matrix_translate( 0.0f, 0.0f, 1.0f );

		// world transformation matrix
		struct LIB_RGL_STRUCTURE_MATRIX w_matrix = lib_rgl_return_matrix_identity();

		// connect all matrixes into one
		w_matrix = lib_rgl_multiply_matrix( &x_matrix, &y_matrix );
		w_matrix = lib_rgl_multiply_matrix( &w_matrix, &z_matrix );

		// amount of faces to sort
		uint64_t sc = 0;

		// convert all face coordinates relative to world matrix
		for( uint64_t i = 1; i <= fc; i++ ) {
			// face to parse
			parse[ i ] = face[ i ];

			// convert
			lib_rgl_multiply( &parse[ i ], &w_matrix );
			lib_rgl_multiply( &parse[ i ], &t_matrix );

			// check if face will be visible
			if( lib_rgl_projection( rgl, &parse[ i ] ) ) {
				// convert to our display frustum
				lib_rgl_multiply( &parse[ i ], &p_matrix );

				// if yes, add to sorting list
				sort[ sc ] = &parse[ i ];

				// calculate average value of depth for 3 points of face
				sort[ sc ] -> z_depth = (parse[ i ].point[ 0 ].z + parse[ i ].point[ 1 ].z + parse[ i ].point[ 2 ].z) / 3.0f;

				sc++;
			}
		}

		// sort faces by Z axis
		lib_rgl_sort_quick( sort, 1, sc - 1 );

		// draw every triangle on workbench
		for( uint64_t i = 0; i < sc; i++ ) lib_rgl_triangle( rgl, sort[ i ], material );

		// synchronize workbench with framebuffer
		lib_rgl_flush( rgl );
	}
}
