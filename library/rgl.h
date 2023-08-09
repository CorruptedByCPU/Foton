/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

typedef	struct VECTOR3F {
	double	x;
	double	y;
	double	z;
	double	w;
} vector3f;

struct	LIB_RGL_STRUCTURE_TRIANGLE {
	vector3f	point[ 3 ];
	double		z_depth;
	uint32_t	color;
};