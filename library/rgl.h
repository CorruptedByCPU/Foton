/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_RGL
	#define	LIB_RGL

        typedef	struct VECTOR2D {
                int64_t	x;
                int64_t	y;
        } vector2d;

	typedef	struct VECTOR3F {
		double	x;
		double	y;
		double	z;
		double	w;
	} vector3f;

	struct	LIB_RGL_STRUCTURE {
		uint16_t	width_pixel;
		uint16_t	height_pixel;
		uint64_t	pitch_byte;
		uint32_t	*base_address;
		uint64_t	size_byte;
		uint32_t	*workbench_base_address;
		uint32_t	color_background;
		vector3f	camera;
	};

	struct	LIB_RGL_STRUCTURE_TRIANGLE {
		vector3f	point[ 3 ];
		double		z_depth;
		uint32_t	color;
	};

	struct LIB_RGL_STRUCTURE_MATRIX {
		double	cell[ 4 ][ 4 ];
	};

	// init function of Raw Graphis Library
	struct LIB_RGL_STRUCTURE *lib_rgl( uint16_t width_pixel, uint16_t height_pixel, uint32_t *base_address );

	// cleans worbench area
	void lib_rgl_clean( struct LIB_RGL_STRUCTURE *rgl );

	// synchronizes workbench area with framebuffer
	void lib_rgl_flush( struct LIB_RGL_STRUCTURE *rgl );

	// returns default matrix content
	struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_identity();

	// multiply triangle by matrix
	void lib_rgl_multiply( struct LIB_RGL_STRUCTURE_TRIANGLE *triangle, struct LIB_RGL_STRUCTURE_MATRIX *matrix );

	// multiply matrixes
	struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_multiply_matrix( struct LIB_RGL_STRUCTURE_MATRIX *this, struct LIB_RGL_STRUCTURE_MATRIX *via );

	uint8_t lib_rgl_projection( struct LIB_RGL_STRUCTURE *rgl, struct LIB_RGL_STRUCTURE_TRIANGLE *parse );

	// returns calculated matrix for rotation X axis
	struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_rotate_x( double a );

	// returns calculated matrix for rotation Y axis
	struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_rotate_y( double a );

	// returns calculated matrix for rotation Z axis
	struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_rotate_z( double a );

	// returns calculated matrix for movement at X, Y, Z axis
	struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_translate( double x, double y, double z );

	double lib_rgl_vector_product_dot( vector3f *v0, vector3f *v1 );
	vector3f lib_rgl_vector_product_cross( vector3f *v0, vector3f *v1 );
	double lib_rgl_vector_length( vector3f *v );
	void lib_rgl_vector_normalize( vector3f *v );
	vector3f lib_rgl_vector_substract( vector3f *from, vector3f *substract );
	uint64_t lib_rgl_partition( struct LIB_RGL_STRUCTURE_TRIANGLE **triangles, uint64_t low, uint64_t high );
	void lib_rgl_sort_quick( struct LIB_RGL_STRUCTURE_TRIANGLE **triangles, uint64_t low, uint64_t high );
	inline uint8_t lib_rgl_edge( vector2d *a, vector2d *b, vector2d *c );
	void lib_rgl_line( struct LIB_RGL_STRUCTURE *rgl, int64_t x0, int64_t y0, int64_t x1, int64_t y1, uint32_t color );
	void lib_rgl_triangle( struct LIB_RGL_STRUCTURE *rgl, struct LIB_RGL_STRUCTURE_TRIANGLE *triangle );
#endif