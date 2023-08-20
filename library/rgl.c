/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_RGL
		#include	"./rgl.h"
	#endif
	#ifndef	LIB_MATH
		#include	"./math.h"
	#endif

struct LIB_RGL_STRUCTURE *lib_rgl( uint16_t width_pixel, uint16_t height_pixel, uint32_t *base_address ) {
	// prepare space for RGL structure
	struct LIB_RGL_STRUCTURE *rgl = (struct LIB_RGL_STRUCTURE *) malloc( sizeof( struct LIB_RGL_STRUCTURE ) );

	// set properties of RGL
	rgl -> width_pixel = width_pixel;
	rgl -> height_pixel = height_pixel;
	rgl -> base_address = base_address;

	// calculate display area size in Bytes
	rgl -> size_byte = (rgl -> width_pixel * rgl -> height_pixel) << STD_VIDEO_DEPTH_shift;

	// prepare workbench area
	rgl -> workbench_base_address = (uint32_t *) malloc( rgl -> size_byte );

	// prepare workbench area
	rgl -> depth_base_address = (double *) malloc( rgl -> width_pixel * rgl -> height_pixel * sizeof( double ) );


	// set default color of RGL area
	rgl -> color_background = STD_COLOR_BLACK;

	// camera position
	rgl -> camera.x = 0.0f;
	rgl -> camera.y = 0.0f;
	rgl -> camera.z = -1.0f;

	// clean up workbench
	lib_rgl_clean( rgl );

	// return RGL specification for future use
	return rgl;
}

void lib_rgl_clean( struct LIB_RGL_STRUCTURE *rgl ) {
	// clean up workbench area
	for( uint64_t y = 0; y < rgl -> height_pixel; y++ )
		for( uint64_t x = 0; x < rgl -> width_pixel; x++ )
			rgl -> workbench_base_address[ (y * rgl -> width_pixel) + x ] = rgl -> color_background;

	double d;
	(*((uint64_t *) &d))= ~(1LL << 52);

	// clean up depth area
	for( uint64_t y = 0; y < rgl -> height_pixel; y++ )
		for( uint64_t x = 0; x < rgl -> width_pixel; x++ )
			rgl -> depth_base_address[ (y * rgl -> width_pixel) + x ] = d;
}

void lib_rgl_flush( struct LIB_RGL_STRUCTURE *rgl ) {
	// synchronize standard output with workbench
	for( uint64_t y = 0; y < rgl -> height_pixel; y++ )
		for( uint64_t x = 0; x < rgl -> width_pixel; x++ )
			rgl -> base_address[ (y * rgl -> width_pixel) + x ] = rgl -> workbench_base_address[ (y * rgl -> width_pixel) + x ];
}

// void lib_rgl_multiply( struct LIB_RGL_STRUCTURE_TRIANGLE *triangle, struct LIB_RGL_STRUCTURE_MATRIX *matrix ) {
// 	// temporary face
// 	struct LIB_RGL_STRUCTURE_TRIANGLE tmp;

// 	// for each point in face
// 	for( uint8_t i = 0; i < 3; i++ ) {
// 		// preserve face properties
// 		tmp.point[ i ] = triangle -> point[ i ];

// 		// convert by matrix
// 		triangle -> point[ i ].x = (tmp.point[ i ].x * matrix -> cell[ 0 ][ 0 ]) + (tmp.point[ i ].y * matrix -> cell[ 1 ][ 0 ]) + (tmp.point[ i ].z * matrix -> cell[ 2 ][ 0 ]) + matrix -> cell[ 3 ][ 0 ];
// 		triangle -> point[ i ].y = (tmp.point[ i ].x * matrix -> cell[ 0 ][ 1 ]) + (tmp.point[ i ].y * matrix -> cell[ 1 ][ 1 ]) + (tmp.point[ i ].z * matrix -> cell[ 2 ][ 1 ]) + matrix -> cell[ 3 ][ 1 ];
// 		triangle -> point[ i ].z = (tmp.point[ i ].x * matrix -> cell[ 0 ][ 2 ]) + (tmp.point[ i ].y * matrix -> cell[ 1 ][ 2 ]) + (tmp.point[ i ].z * matrix -> cell[ 2 ][ 2 ]) + matrix -> cell[ 3 ][ 2 ];
	
// 		double w = (tmp.point[ i ].x * matrix -> cell[ 0 ][ 3 ]) + (tmp.point[ i ].y * matrix -> cell[ 1 ][ 3 ]) + (tmp.point[ i ].z * matrix -> cell[ 2 ][ 3 ]) + matrix -> cell[ 3 ][ 3 ];

// 		if( w != 0.0f ) {
// 			triangle -> point[ i ].x /= w;
// 			triangle -> point[ i ].y /= w;
// 			triangle -> point[ i ].z /= w;
// 		}
// 	}
// }

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_multiply_matrix( struct LIB_RGL_STRUCTURE_MATRIX *this, struct LIB_RGL_STRUCTURE_MATRIX *via ) {
	struct LIB_RGL_STRUCTURE_MATRIX tmp;

	for (int y = 0; y < 4; y++)
		for (int x = 0; x < 4; x++)
			tmp.cell[ x ][ y ] = this -> cell[ x ][ 0 ] * via -> cell[ 0 ][ y ] + this -> cell[ x ][ 1 ] * via -> cell[ 1 ][ y ] + this -> cell[ x ][ 2 ] * via -> cell[ 2 ][ y ] + this -> cell[ x ][ 3 ] * via -> cell[ 3 ][ y ];

	return tmp;
}

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_identity() {
	struct LIB_RGL_STRUCTURE_MATRIX matrix;

	matrix.cell[ 0 ][ 0 ] = 1.0f;
	matrix.cell[ 1 ][ 1 ] = 1.0f;
	matrix.cell[ 2 ][ 2 ] = 1.0f;
	matrix.cell[ 3 ][ 3 ] = 1.0f;

	return matrix;
}

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_rotate_x( double a ) {
	struct LIB_RGL_STRUCTURE_MATRIX matrix = lib_rgl_return_matrix_identity();

	matrix.cell[ 0 ][ 0 ] = 1.0f;
	matrix.cell[ 1 ][ 1 ] = cos( a );
	matrix.cell[ 1 ][ 2 ] = -sin( a );
	matrix.cell[ 2 ][ 1 ] = sin( a );
	matrix.cell[ 2 ][ 2 ] = cos( a );
	matrix.cell[ 3 ][ 3 ] = 1.0f;

	return matrix;
}

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_rotate_y( double a ) {
	struct LIB_RGL_STRUCTURE_MATRIX matrix = lib_rgl_return_matrix_identity();

	matrix.cell[ 0 ][ 0 ] = cos( a );
	matrix.cell[ 0 ][ 2 ] = sin( a );
	matrix.cell[ 2 ][ 0 ] = -sin( a );
	matrix.cell[ 1 ][ 1 ] = 1.0f;
	matrix.cell[ 2 ][ 2 ] = cos( a );
	matrix.cell[ 3 ][ 3 ] = 1.0f;

	return matrix;
}

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_rotate_z( double a ) {
	struct LIB_RGL_STRUCTURE_MATRIX matrix = lib_rgl_return_matrix_identity();

	matrix.cell[ 0 ][ 0 ] = cos( a );
	matrix.cell[ 0 ][ 1 ] = -sin( a );
	matrix.cell[ 1 ][ 0 ] = sin( a );
	matrix.cell[ 1 ][ 1 ] = cos( a );
	matrix.cell[ 2 ][ 2 ] = 1.0f;
	matrix.cell[ 3 ][ 3 ] = 1.0f;

	return matrix;
}

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_translate( double x, double y, double z ) {
	struct LIB_RGL_STRUCTURE_MATRIX matrix = lib_rgl_return_matrix_identity();

	matrix.cell[ 3 ][ 0 ] = x;
	matrix.cell[ 3 ][ 1 ] = y;
	matrix.cell[ 3 ][ 2 ] = z;

	return matrix;
}

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_scale( double x, double y, double z ) {
	struct LIB_RGL_STRUCTURE_MATRIX matrix = lib_rgl_return_matrix_identity();

	matrix.cell[ 0 ][ 0 ] = x;
	matrix.cell[ 1 ][ 1 ] = y;
	matrix.cell[ 2 ][ 2 ] = z;

	return matrix;
}

double lib_rgl_vector_product_dot( vector3f *v0, vector3f *v1 ) {
	return (v0 -> x * v1 -> x) + (v0 -> y * v1 -> y) + (v0 -> z * v1 -> z);
}

vector3f lib_rgl_vector_product_cross( vector3f *v0, vector3f *v1 ) {
	vector3f tmp;

	tmp.x = (v0 -> y * v1 -> z) - (v0 -> z * v1 -> y);
	tmp.y = (v0 -> z * v1 -> x) - (v0 -> x * v1 -> z);
	tmp.z = (v0 -> x * v1 -> y) - (v0 -> y * v1 -> x);

	return tmp;
}

double lib_rgl_vector_length( vector3f *v ) {
	return (double) sqrtf( (float) lib_rgl_vector_product_dot( v, v ) );
}

void lib_rgl_vector_normalize( vector3f *v ) {
	double length = lib_rgl_vector_length( v );

	v -> x /= length;
	v -> y /= length;
	v -> z /= length;
}

vector3f lib_rgl_vector_substract( vector3f *from, vector3f *substract ) {
	vector3f tmp;

	tmp.x = from -> x - substract -> x;
	tmp.y = from -> y - substract -> y;
	tmp.z = from -> z - substract -> z;

	return tmp;
}

uint8_t lib_rgl_projection( struct LIB_RGL_STRUCTURE *rgl, vector3f *vr, struct LIB_RGL_STRUCTURE_TRIANGLE *parse ) {
	// if( vr[ parse -> v[ 0 ] ].z < 0.0f || vr[ parse -> v[ 1 ] ].z < 0.0f || vr[ parse -> v[ 2 ] ].z < 0.0f ) return FALSE;

	vector3f line1 = lib_rgl_vector_substract( (vector3f *) &vr[ parse -> v[ 1 ] ], (vector3f *) &vr[ parse -> v[ 0 ] ] );
	vector3f line2 = lib_rgl_vector_substract( (vector3f *) &vr[ parse -> v[ 2 ] ], (vector3f *) &vr[ parse -> v[ 0 ] ] );

	vector3f normal = lib_rgl_vector_product_cross( (vector3f *) &line1, (vector3f *) &line2 );

	lib_rgl_vector_normalize( (vector3f *) &normal );

	vector3f camera_ray = lib_rgl_vector_substract( (vector3f *) &vr[ parse -> v[ 0 ] ], (vector3f *) &rgl -> camera );

	// show only visible triangles
	if( lib_rgl_vector_product_dot( (vector3f *) &normal, (vector3f *) &camera_ray ) < 0.0f ) {
	// if( normal.z < 0.0f ) {
		// light source position
		vector3f light = { 0.0f, 0.0f, -1.0f };
		lib_rgl_vector_normalize( (vector3f *) &light );

		// dot product
		parse -> light = maxf( 0.1f, lib_rgl_vector_product_dot( (vector3f *) &normal, (vector3f *) &light ) ) / 4.0f;

		// triangle visible
		return TRUE;
	}

	// triangle invisible
	return FALSE;
}

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_projection( struct LIB_RGL_STRUCTURE *rgl, double fov ) {
	struct LIB_RGL_STRUCTURE_MATRIX matrix = lib_rgl_return_matrix_identity();

	double n = 1.0f;
	double f = 10.0f;
	double rad = 1.0f / tan( (fov * 0.5f) / (LIB_MATH_PI * 180.0f) );

	matrix.cell[ 0 ][ 0 ] = rad * ((double) rgl -> height_pixel / (double) rgl -> width_pixel);
	matrix.cell[ 1 ][ 1 ] = rad / ((double) rgl -> width_pixel / (double) rgl -> height_pixel);
	matrix.cell[ 2 ][ 2 ] = -(f / (f - n));
	matrix.cell[ 2 ][ 3 ] = 1.0f;
	matrix.cell[ 3 ][ 2 ] = -(f * n) / (f - n);
	matrix.cell[ 3 ][ 3 ] = 0.0f;

	// matrix.cell[ 0 ][ 0 ] = (2.0f * n) / (double) rgl -> width_pixel;
	// matrix.cell[ 1 ][ 1 ] = (2.0f * n) / (double) rgl -> height_pixel;
	// matrix.cell[ 2 ][ 2 ] = f / (f - n);
	// matrix.cell[ 2 ][ 3 ] = 1.0f;
	// matrix.cell[ 3 ][ 2 ] = -f * n / (f - n);
	// matrix.cell[ 3 ][ 3 ] = 0.0f;

	return matrix;
}

void lib_rgl_multiply_vector( vector3f *v, struct LIB_RGL_STRUCTURE_MATRIX *matrix ) {
	// temporary vector
	vector3f t = *v;

	// convert by matrix
	v -> x = (t.x * matrix -> cell[ 0 ][ 0 ]) + (t.y * matrix -> cell[ 1 ][ 0 ]) + (t.z * matrix -> cell[ 2 ][ 0 ]) + matrix -> cell[ 3 ][ 0 ];
	v -> y = (t.x * matrix -> cell[ 0 ][ 1 ]) + (t.y * matrix -> cell[ 1 ][ 1 ]) + (t.z * matrix -> cell[ 2 ][ 1 ]) + matrix -> cell[ 3 ][ 1 ];
	v -> z = (t.x * matrix -> cell[ 0 ][ 2 ]) + (t.y * matrix -> cell[ 1 ][ 2 ]) + (t.z * matrix -> cell[ 2 ][ 2 ]) + matrix -> cell[ 3 ][ 2 ];

	double w = (t.x * matrix -> cell[ 0 ][ 3 ]) + (t.y * matrix -> cell[ 1 ][ 3 ]) + (t.z * matrix -> cell[ 2 ][ 3 ]) + matrix -> cell[ 3 ][ 3 ];

	if( w != 0.0f ) {
		v -> x /= w;
		v -> y /= w;
		v -> z /= w;
	}
}