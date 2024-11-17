/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// file name provided?
	if( argc > 1 )	{
		// search for file names
		for( uint64_t i = 1; i < argc; i++ ) {
			// ignore any passed options, no support yet
			if( argv[ i ][ 0 ] == '-' ) continue;
			else {
				// create directory of definied name
				FILE *dir = touch( argv[ i ], STD_FILE_TYPE_directory );

				// if created
				if( dir ) fclose( dir );	// close it
			}
		}
	}

	// exit
	return 0;
}
