/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_MATH
	#define	LIB_MATH

	#define	LIB_MATH_PI (3.14159265358979323846264338327950288419716939937510)
	#define	LIB_MATH_PI_2x (6.28318530717958623199592693708837032318115234375000)
	#define	LIB_MATH_EPSILON (0.000000000000000000000000000000000000000000000000001)

	int8_t lib_math_compare_double( double f1, double f2 );
	double lib_math_cos( double x );
	double lib_math_fabs( double x );
	double lib_math_fmod( double x, double y );
	double lib_math_sin( double x );
	double lib_math_tan( double x );
	double lib_math_ctan( double x );
	double lib_math_sign( double x );
#endif
