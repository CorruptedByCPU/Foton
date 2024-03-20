/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	<dirent.h>
	#include	<stdint.h>
	#include	<stdio.h>
	#include	<stdlib.h>
	#include	<string.h>
	#include	<sys/stat.h>
	#include	<sys/types.h>
	#include	<dirent.h>
	// library -------------------------------------------------------------
	#include	"../library/vfs.h"
	#include	"../library/elf.h"
	#include	"../library/macro.h"
	//======================================================================

	#define	EMPTY				0

	#define	STD_FILE_TYPE_default	0b00000001
	#define	STD_FILE_TYPE_directory	0b00000010
	#define	STD_FILE_TYPE_link		0b00000100

	#define	STD_PAGE_byte			0x1000
	#define	STD_PAGE_mask			0xFFFFFFFFFFFFF000

char path_export[] = "./";
char file_extension[] = ".vfs";
char name_symlink[] = "..";

char file_so[ sizeof( struct LIB_ELF_STRUCTURE ) ];
struct LIB_ELF_STRUCTURE *elf = (struct LIB_ELF_STRUCTURE *) &file_so;

DIR *isdir;

void vfs_default( struct LIB_VFS_STRUCTURE *vfs ) {
	// prepare default symlinks for root directory
	vfs[ 0 ].offset = EMPTY;
	vfs[ 0 ].name_length = 1;
	vfs[ 0 ].type = STD_FILE_TYPE_link;
	strncpy( (char *) &vfs[ 0 ].name, name_symlink, 1 );
	vfs[ 1 ].offset = EMPTY;
	vfs[ 1 ].name_length = 2;
	vfs[ 1 ].type = STD_FILE_TYPE_link;
	strncpy( (char *) &vfs[ 1 ].name, name_symlink, 2 );
}

int main( int argc, char *argv[] ) {
	// arg_length
	uint64_t arg_length = strlen( argv[ 1 ] );
	if( argv[ 1 ][ arg_length - 1 ] == '/' ) argv[ 1 ][ arg_length ] = EMPTY;

	// prepare import path
	char path_import[ arg_length + 1 ];
	snprintf( path_import, sizeof( path_import ), "%s%c", argv[ 1 ], 0x2F );

	// prepare vfs header
	struct LIB_VFS_STRUCTURE *vfs = malloc( sizeof( struct LIB_VFS_STRUCTURE ) * LIB_VFS_default );
	memset( vfs, 0, sizeof( *vfs ) );

	// create default symlinks
	vfs_default( vfs );

	// included files
	uint64_t files_included = LIB_VFS_default;

	// directory entry
	struct dirent *entry = NULL;

	// open directory content
	DIR *directory = opendir( argv[ 1 ] );

	// for every file inside directory
	while( (entry = readdir( directory )) != NULL ) {
		// ignore system files
		if( ! strcmp( entry -> d_name, "." ) || ! strcmp( entry -> d_name, ".." ) ) continue;

		// file name longer than limit?
		if( strlen( entry -> d_name ) > LIB_VFS_NAME_limit ) { printf( " [name \"%s\" too long]\n", entry -> d_name ); return -1; }

		// resize header for new file
		vfs = realloc( vfs, sizeof( struct LIB_VFS_STRUCTURE ) * (files_included + 1) );

		// clean up new entry space (Linux doesn't quarantee it)
		uint8_t *new = (uint8_t *) &vfs[ files_included ];
		for( uint16_t i = 0; i < sizeof( struct LIB_VFS_STRUCTURE ); i++ ) new[ i ] = EMPTY;

		// insert: name, length
		vfs[ files_included ].name_length = strlen( entry -> d_name );
		strcpy( (char *) vfs[ files_included ].name, entry -> d_name );

		// combine path to file
		char path_local[ sizeof( argv[ 1 ] ) + LIB_VFS_NAME_limit + 1 ];
		snprintf( path_local, sizeof( path_local ), "%s%c%s", path_import, 0x2F, vfs[ files_included ].name );

		// Insert

		// size of file in Bytes
		struct stat finfo;
		stat( (char *) path_local, &finfo );	// get file specification
		vfs[ files_included ].byte = finfo.st_size;

		// type is directory?
		if( (isdir = opendir( path_local )) != NULL ) {
			// prepare directory path
			char path_directory[ 6 + sizeof( argv[ 1 ] ) + LIB_VFS_NAME_limit ];
			snprintf( path_directory, sizeof( path_directory ), "./vfs %s/%s internal", argv[ 1 ], entry -> d_name );

			// prepare subdirectory file structure
			system( path_directory );

			// combine path to file
			char path_insert[ sizeof( argv[ 1 ] ) + LIB_VFS_NAME_limit + 1];
			snprintf( path_insert, sizeof( path_insert ), "%s/%s.vfs", argv[ 1 ], entry -> d_name );

			// size of file in Bytes
			struct stat finfo;
			stat( (char *) path_insert, &finfo );	// get file specification
			vfs[ files_included ].byte = finfo.st_size;

			vfs[ files_included++ ].type = STD_FILE_TYPE_directory;

			continue;
		}

		// type
		vfs[ files_included ].type = STD_FILE_TYPE_default;

		// next directory entry
		files_included++;
	}

	// we don't need it anymore, close it up
	closedir( directory );

	// resize header for new file
	vfs = realloc( vfs, sizeof( struct LIB_VFS_STRUCTURE ) * (files_included + 1) );

	// last entry keep as empty
	uint8_t *last = (uint8_t *) &vfs[ files_included++ ];
	for( uint16_t i = 0; i < sizeof( struct LIB_VFS_STRUCTURE ); i++ ) last[ i ] = EMPTY;

	// offset of first file inside package
	uint64_t offset = MACRO_PAGE_ALIGN_UP( sizeof( struct LIB_VFS_STRUCTURE ) * files_included );

	// calculate offset for every registered file
	for( uint64_t i = LIB_VFS_default; i < files_included - 1; i++ ) {
		// first file
		vfs[ i ].offset = offset;

		// next file (align file position)
		offset += MACRO_PAGE_ALIGN_UP( vfs[ i ].byte );
	}

	// update size of root directory inside symlinks
	for( uint8_t i = 0; i < LIB_VFS_default; i++ ) vfs[ i ].byte = MACRO_PAGE_ALIGN_UP( sizeof( struct LIB_VFS_STRUCTURE ) * files_included );

	/*--------------------------------------------------------------------*/

	// combine path to file
	char path_local[ sizeof( path_export ) + sizeof( argv[ 1 ] ) + LIB_VFS_NAME_limit + sizeof( file_extension ) ];
	snprintf( path_local, sizeof( path_local ), "%s%s%s", path_export, argv[ 1 ], file_extension );

	// open new package for write
	FILE *fvfs = fopen( path_local, "w" );

	// append file header
	uint64_t size = sizeof( struct LIB_VFS_STRUCTURE ) * files_included;	// last data offset in Bytes
	fwrite( vfs, size, 1, fvfs );

	// append files described in header
	for( uint64_t i = LIB_VFS_default; i < files_included - 1; i++ ) {
		// align file to offset
		for( uint64_t j = 0; j < MACRO_PAGE_ALIGN_UP( size ) - size; j++ ) fputc( '\x00', fvfs );

		if( vfs[ i ].type & STD_FILE_TYPE_directory ) {
			// combine path to file
			char path_insert[ sizeof( path_import ) + LIB_VFS_NAME_limit + 1];
			snprintf( path_insert, sizeof( path_insert ), "%s/%s.vfs", path_import, vfs[ i ].name );

			// append file to package
			FILE *file = fopen( path_insert, "r" );
			for( uint64_t f = 0; f < vfs[ i ].byte; f++ ) fputc( fgetc( file ), fvfs );
			fclose( file );
		} else {
			// combine path to file
			char path_insert[ sizeof( path_import ) + LIB_VFS_NAME_limit + 1 ];
			snprintf( path_insert, sizeof( path_insert ), "%s/%s", path_import, vfs[ i ].name );

			// append file to package
			FILE *file = fopen( path_insert, "r" );
			for( uint64_t f = 0; f < vfs[ i ].byte; f++ ) fputc( fgetc( file ), fvfs );
			fclose( file );
		}

		// last data offset in Bytes
		size = vfs[ i ].offset + vfs[ i ].byte;
	}

	// release header
	free( vfs );

	// append magic value only to root directory
	if( argc == 2 ) {
		// align magic value to uint32_t size
		for( uint8_t a = 0; a < sizeof( uint32_t ) - (size % sizeof( uint32_t )); a++ ) fputc( '\x00', fvfs );

		// append magic value to end of vfs file
		uint32_t magic = LIB_VFS_magic;
		fwrite( &magic, LIB_VFS_length, 1, fvfs );
	}

	// close package
	fclose( fvfs );

	// package created
	return 0;
}