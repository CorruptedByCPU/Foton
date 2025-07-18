/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_RGL
	#define	LIB_RGL

	#ifndef	LIB_STRING
		#include	"./math.h"
	#endif

        typedef	struct VECTOR2D {
                int64_t	x;
                int64_t	y;
	} vector2d;

        typedef	struct VECTOR2D1F {
                int64_t	x;
                int64_t	y;
		double	z;
        } vector2d1f;

	typedef	struct VECTOR2F {
		double	x;
		double	y;
	} vector2f;

	typedef	struct VECTOR3F {
		double	x;
		double	y;
		double	z;
		double	w;
	} vector3f;

	struct	LIB_RGL_STRUCTURE {
		uint16_t	width_pixel;
		uint16_t	height_pixel;
		uint32_t	scanline_pixel;
		uint32_t	*base_address;
		uint64_t	size_byte;
		uint32_t	*workbench_base_address;
		double		*depth_base_address;
		uint32_t	color_background;
		uint8_t		color_alpha;
		vector3f	camera;
		vector3f	target;
		vector3f	ambient_light;
	};

	struct	LIB_RGL_STRUCTURE_MATERIAL {
		vector3f	Ka;
		vector3f	Kd;
		uint8_t		length;
		uint8_t		name[ 255 ];
	};

	struct	LIB_RGL_STRUCTURE_TRIANGLE {
		uint64_t	v[ 3 ];
		double		z_depth;
		uint64_t	material;
		double		light;
	};

	struct LIB_RGL_STRUCTURE_MATRIX {
		double	cell[ 4 ][ 4 ];
	};

	// init function of Raw Graphis Library
	struct LIB_RGL_STRUCTURE *lib_rgl( uint16_t width_pixel, uint16_t height_pixel, uint32_t scanline_pixel, uint32_t *base_address );

	// cleans worbench area
	void lib_rgl_clean( struct LIB_RGL_STRUCTURE *rgl );

	// synchronizes workbench area with framebuffer
	void lib_rgl_flush( struct LIB_RGL_STRUCTURE *rgl );

	// returns default matrix content
	struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_identity( void );

	// multiply triangle by matrix
	void lib_rgl_multiply( struct LIB_RGL_STRUCTURE_TRIANGLE *triangle, struct LIB_RGL_STRUCTURE_MATRIX *matrix );

	// multiply matrixes
	struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_multiply_matrix( struct LIB_RGL_STRUCTURE_MATRIX this, struct LIB_RGL_STRUCTURE_MATRIX via );

	uint8_t lib_rgl_projection( struct LIB_RGL_STRUCTURE *rgl, vector3f *vr, struct LIB_RGL_STRUCTURE_TRIANGLE *parse );

	// returns calculated matrix for rotation X axis
	struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_rotate_x( double a );

	// returns calculated matrix for rotation Y axis
	struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_rotate_y( double a );

	// returns calculated matrix for rotation Z axis
	struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_rotate_z( double a );

	// returns calculated matrix for movement at X, Y, Z axis
	struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_translate( double x, double y, double z );

	struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_projection( struct LIB_RGL_STRUCTURE *rgl );

	double lib_rgl_vector_product_dot( vector3f v0, vector3f v1 );
	vector3f lib_rgl_vector_product_cross( vector3f v0, vector3f v1 );
	struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_scale( double x, double y, double z );
	double lib_rgl_vector_length( vector3f v );
	vector3f lib_rgl_return_vector_normalize( vector3f v );
	struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_view( struct LIB_RGL_STRUCTURE *rgl );
	vector3f lib_rgl_vector_substract( vector3f from, vector3f substract );
	uint64_t lib_rgl_partition( struct LIB_RGL_STRUCTURE_TRIANGLE **triangles, uint64_t low, uint64_t high );
	void lib_rgl_sort_quick( struct LIB_RGL_STRUCTURE_TRIANGLE **triangles, uint64_t low, uint64_t high );
	void lib_rgl_line( struct LIB_RGL_STRUCTURE *rgl, int64_t x0, int64_t y0, int64_t x1, int64_t y1, uint32_t color );
	void lib_rgl_multiply_vector( vector3f *v, struct LIB_RGL_STRUCTURE_MATRIX *matrix );
	inline uint8_t lib_rgl_edge( vector2d *a, vector2d *b, vector2d *c );
	struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_perspective( struct LIB_RGL_STRUCTURE *rgl, double fov, double aspect, double n, double f );
	uint32_t lib_rgl_color( struct LIB_RGL_STRUCTURE *rgl, vector3f Ka, vector3f Kd, double light );
	void lib_rgl_triangle( struct LIB_RGL_STRUCTURE *rgl, struct LIB_RGL_STRUCTURE_TRIANGLE *t, vector3f *vp, struct LIB_RGL_STRUCTURE_MATERIAL *material );
	void lib_rgl_fill( struct LIB_RGL_STRUCTURE *rgl, struct LIB_RGL_STRUCTURE_TRIANGLE *t, vector3f *vp, struct LIB_RGL_STRUCTURE_MATERIAL *material );
	void lib_rgl_2d_square( struct LIB_RGL_STRUCTURE *rgl, int64_t x, int64_t y, uint64_t width, uint64_t height, uint32_t color );
	void lib_rgl_resize( struct LIB_RGL_STRUCTURE *rgl, uint16_t width_pixel, uint16_t height_pixel, uint32_t scanline_pixel, uint32_t *base_address );
#endif
