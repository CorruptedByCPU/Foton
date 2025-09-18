/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<stdint.h>

int main( int argc, char *argv[] ) {
	// open file only for read
	FILE *read = fopen( argv[ 1 ], "r" );

	// calculate file size in Bytes
	fseek( read, 0, SEEK_END );
	uint64_t size = ftell( read );
	fseek( read, 0, SEEK_SET );

	// read file content to array
	uint64_t v = 0;
	uint8_t data[ size ];
	while( fscanf( read, "%c", &data[ v++ ] ) != EOF );

	// close file, we don't need it anymore
	fclose( read );

	// we need only information about alpha channel
	uint64_t pixels = size / 4;
	uint8_t alpha[ pixels ];
	for( uint64_t p = 0; p < pixels; p++ )
		alpha[ p ] = data[ (p * 4) + 3 ];

	// save encoded alpha channel to new file as 
	FILE *write = fopen( "font.h", "w" );

	// array definition
	fprintf( write, "uint8_t font_matrix[ %u ] = {\n\t", (unsigned int) pixels );

	// include all alpha channel bytes
	for( uint64_t e = 0; e < pixels; e++ ) {
		fprintf( write, "0x%02X", alpha[ e ] );
		if( e < pixels - 1 ) fprintf( write, ", " );
		else fprintf( write, "\n}\n" );
	}

	// done
	fclose( write );
}
