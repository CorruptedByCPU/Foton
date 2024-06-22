/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void top_size( uint64_t bytes ) {
	// unity type
	uint8_t unit = 0;	// bytes by default

	// calculate unit type
	double value = (double) bytes;
	while( value >= (double) 1024.0f ) { value /= (double) 1024.0f; unit++; }

	// show value with unit
	printf( "%4.1f%c", value, top_units[ unit ] );
}