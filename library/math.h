/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_MATH
	#define	LIB_MATH

	#define	LIB_MATH_PI (3.14159265358979323846264338327950288419716939937510)
	#define	LIB_MATH_PI_2x (6.28318530717958623199592693708837032318115234375000)
	#define	LIB_MATH_EPSILON (0.000000000000000000000000000000000000000000000000001)

	int8_t lib_math_compare_double( double f1, double f2 );
	double cos( double x );
	double sin( double x );
	double tan( double x );
	double ctan( double x );
#endif