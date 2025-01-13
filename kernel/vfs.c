/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uintptr_t kernel_vfs_block_by_id( struct LIB_VFS_STRUCTURE *vfs, uint64_t i ) {
	// direct block?
	if( i <= 12 ) return vfs -> offset[ i ];	// return pointer

	// indirect block?
	if( i > 12 && i < 524 ) {
		// properties of indirect block
		uintptr_t *indirect = (uintptr_t *) vfs -> offset[ 13 ];

		// return pointer
		return indirect[ i - 13 ];
	}

	// no support for block id, yet
	return EMPTY;
}

uintptr_t kernel_vfs_block_fill( struct LIB_VFS_STRUCTURE *vfs, uint64_t i ) {
	// direct blocks
	for( uint64_t b = 0; b < 13; b++ ) {
		// that was last block
		if( ! i-- ) return vfs -> offset[ b ];

		// block exist?
		if( vfs -> offset[ b ] ) continue;	// yep

		// allocate block
		vfs -> offset[ b ] = kernel_memory_alloc( TRUE );
	}

	// indirect block exist?
	if( ! vfs -> offset[ 13 ] ) kernel_memory_alloc( TRUE );	// no, add

	// indirect blocks
	uintptr_t *indirect = (uintptr_t *) vfs -> offset[ 13 ];
	for( uint64_t b = 0; b < 512; b++ ) {
		// that was last block
		if( ! i-- ) return vfs -> offset[ b ];

		// block exist?
		if( indirect[ b ] ) continue;	// yep

		// allocate block
		indirect[ b ] = kernel_memory_alloc( TRUE );
	}

	// we should not be in here
	return EMPTY;
}

uintptr_t kernel_vfs_block_remove( struct LIB_VFS_STRUCTURE *vfs, uint64_t i ) {
	// removed block pointer
	uintptr_t block = EMPTY;

	// direct block?
	if( i <= 12 ) {
		// preserve block pointer
		block = vfs -> offset[ i ];

		// remove it
		vfs -> offset[ i ] = EMPTY;
	}

	// indirect block?
	if( i > 12 && i < 524 ) {
		// properties of indirect block
		uintptr_t *indirect = (uintptr_t *) vfs -> offset[ 13 ];

		// preserve block pointer
		block = indirect[ i ];

		// remove it
		indirect[ i ] = EMPTY;
	}

	// released block pointer
	return block;
}

void kernel_vfs_file_close( struct KERNEL_STRUCTURE_VFS *socket ) {
	// can we close file?
	if( socket -> pid != kernel_task_pid() ) return;	// no! TODO: something nasty

	// release socket
	socket -> pid = EMPTY;
}

struct KERNEL_STRUCTURE_VFS *kernel_vfs_file_open( uint8_t *path, uint64_t length, uint8_t mode ) {
	// properties of found file
	struct LIB_VFS_STRUCTURE *vfs;
	if( ! (vfs = kernel_vfs_path( path, length )) ) return EMPTY;	// file not found

	// lock exclusive access
	MACRO_LOCK( kernel -> vfs_semaphore );

	// open socket
	struct KERNEL_STRUCTURE_VFS *socket = kernel_vfs_socket_add( vfs );

	// file located on definied storage
	socket -> storage = kernel -> storage_root;

	// file identificator
	socket -> knot = vfs;

	// socket opened by process with ID
	socket -> pid = kernel_task_pid();

	// protect file against any modifications?
	if( mode == STD_FILE_MODE_modify ) {
		// insert ID of process, locked by
		socket -> knot -> lock = socket -> pid;

		// return flag
		socket -> mode = STD_FILE_MODE_modify;
	}

	// unlock access
	MACRO_UNLOCK( kernel -> vfs_semaphore );

	// file found
	return socket;
}

void kernel_vfs_file_properties( struct KERNEL_STRUCTURE_VFS *socket, struct KERNEL_STRUCTURE_VFS_PROPERTIES *properties ) {
	// properties of file
	struct LIB_VFS_STRUCTURE *file = socket -> knot;

	// retrun file size in Bytes
	properties -> byte = file -> byte;

	// return file name
	properties -> name_length = file -> name_length;
	for( uint64_t i = 0; i < file -> name_length; i++ ) properties -> name[ i ] = file -> name[ i ];
}

void kernel_vfs_file_read( struct KERNEL_STRUCTURE_VFS *socket, uint8_t *target, uint64_t seek, uint64_t byte ) {
	// properties of file
	struct LIB_VFS_STRUCTURE *file = socket -> knot;

	// invalid read request?
	if( seek + byte > file -> byte ) {
		// read up to end of file?
		if( seek < file -> byte ) byte = file -> byte - seek;
		else return;	// no, ignore
	}

	// calculate first block number
	uint64_t b = MACRO_PAGE_ALIGN_DOWN( seek ) >> STD_SHIFT_PAGE;

	// read first part of file
	seek %= STD_PAGE_byte;

	// source block pointer
	uint8_t *source;

	// read all blocks containing requested data
	while( byte ) {
		// block containing first part of data
		source = (uint8_t *) kernel_vfs_block_by_id( file, b++ );

		// full or part of block?
		uint64_t limit = STD_PAGE_byte;
		if( limit > byte ) { limit = byte; byte = EMPTY; }
		else byte -= limit;

		// copy data from block
		for( uint64_t i = seek; i < limit; i++ ) *(target++) = source[ i ];

		// start from begining of next block
		seek = EMPTY;
	}
}

struct KERNEL_STRUCTURE_VFS *kernel_vfs_file_create( struct LIB_VFS_STRUCTURE *directory, struct LIB_VFS_STRUCTURE *vfs, uint8_t *name, uint64_t length, uint8_t type ) {
	// new file?
	if( ! vfs -> name_length ) {	// yes
		// set file name
		vfs -> name_length = EMPTY;
		for( uint8_t j = 0; j < length; j++ ) vfs -> name[ vfs -> name_length++ ] = name[ j ];

		// set file type
		vfs -> type = type;

		// default block content
		switch( type ) {
			// for file of type: directory
			case STD_FILE_TYPE_directory: {
				// assign first block for directory
				vfs -> offset[ FALSE ] = kernel_memory_alloc( TRUE );

				// directory default block properties
				struct LIB_VFS_STRUCTURE *dir = (struct LIB_VFS_STRUCTURE *) vfs -> offset[ FALSE ];

				// prepare default symlinks for root directory

				// current location
				dir[ FALSE ].offset[ FALSE ]	= (uintptr_t) vfs;
				dir[ FALSE ].type		= STD_FILE_TYPE_link;
				dir[ FALSE ].name_length	= 1;
				dir[ FALSE ].name[ FALSE ]	= '.';
				
				// previous location
				dir[ TRUE ].offset[ FALSE ]	= (uintptr_t) directory;
				dir[ TRUE ].type		= STD_FILE_TYPE_link;
				dir[ TRUE ].name_length		= 2;
				dir[ TRUE ].name[ FALSE ]	= '.'; dir[ TRUE ].name[ TRUE ] = '.';

				// default directory size
				vfs -> byte = STD_PAGE_byte;

				// done
				break;
			}

			// for file
			default: {
				// clean data pointer
				vfs -> offset[ FALSE ] = EMPTY;

				// and file size
				vfs -> byte = EMPTY;
			}
		}
	}

	// open socket
	struct KERNEL_STRUCTURE_VFS *socket = kernel_vfs_socket_add( vfs );

	// file located on definied storage
	socket -> storage = kernel -> storage_root;

	// file identificator
	socket -> knot = vfs;

	// socket opened by process with ID
	socket -> pid = kernel_task_pid();

	// return new socket
	return socket;
}

struct KERNEL_STRUCTURE_VFS *kernel_vfs_file_touch( uint8_t *path, uint8_t type ) {
	// retrieve path length
	uint64_t length = lib_string_length( path );

	// unsupported length?
	if( ! length ) return EMPTY;	// cannot resolve path

	// pointer to last file name inside path
	uint8_t *file_name = lib_string_basename( path );

	// file name length
	uint64_t file_name_length = length - ((uintptr_t) file_name - (uintptr_t) path);

	// open destination directory
	struct LIB_VFS_STRUCTURE *directory;
	if( ! (directory = kernel_vfs_path( path, length - file_name_length )) ) return EMPTY;	// path not resolvable

	// check if file already exist
	struct LIB_VFS_STRUCTURE *file;
	if( (file = kernel_vfs_path( path, length )) ) return kernel_vfs_file_create( directory, file, file_name, file_name_length, type );

	// for each data block of directory
	uint64_t blocks = directory -> byte >> STD_SHIFT_PAGE;
	for( uint64_t b = 0; b < blocks; b++ ) {
		// first directory block entries
		struct LIB_VFS_STRUCTURE *entry = (struct LIB_VFS_STRUCTURE *) kernel_vfs_block_by_id( directory, b );

		// for every possible entry
		for( uint8_t e = 0; e < STD_PAGE_byte / sizeof( struct LIB_VFS_STRUCTURE ); e++ )
			// if free entry, found
			if( ! entry[ e ].name_length ) return kernel_vfs_file_create( directory, (struct LIB_VFS_STRUCTURE *) &entry[ e ], file_name, file_name_length, type );

		// extend search?
		if( ! kernel_vfs_block_by_id( directory, b + 1 ) ) {
			// expand directory content by block
			kernel_vfs_block_fill( directory, ++blocks );

			// new directory size
			directory -> byte += STD_PAGE_byte;
		}
	}

	// no free entry
	return EMPTY;
}

void kernel_vfs_file_write( struct KERNEL_STRUCTURE_VFS *socket, uint8_t *source, uint64_t seek, uint64_t byte ) {
	// properties of file
	struct LIB_VFS_STRUCTURE *file = socket -> knot;

	// insufficient file length?
	if( seek + byte > MACRO_PAGE_ALIGN_UP( file -> byte ) ) {
		// assign required blocks
		kernel_vfs_block_fill( file, MACRO_PAGE_ALIGN_UP( seek + byte ) >> STD_SHIFT_PAGE );
	}

	// set new file length
	file -> byte = seek + byte;

	// INFO: writing to file from seek == EMPTY, means the same as create new content

	// calculate first block number
	uint64_t b = MACRO_PAGE_ALIGN_DOWN( seek ) >> STD_SHIFT_PAGE;

	// write first part of data
	seek %= STD_PAGE_byte;

	// target block pointer
	uint8_t *target;

	// write all blocks with provided data
	while( byte ) {
		// block for first part of data
		target = (uint8_t *) kernel_vfs_block_by_id( file, b++ );

		// full or part of block?
		uint64_t limit = STD_PAGE_byte;
		if( limit > byte ) limit = byte;

		// copy data to block
		for( uint64_t i = seek; i < limit && byte--; i++ ) target[ i ] = *(source++);

		// start from begining of next block
		seek = EMPTY;
	}

	// truncate no more needed file blocks
	do {
		// obtain block to truncate
		uintptr_t block = kernel_vfs_block_remove( file, b );

		// no more blocks?
		if( ! block ) break;	// yes

		// release it
		kernel_memory_release( block, TRUE );
	// until forever
	} while( TRUE );
}

uint8_t	kernel_vfs_identify( uintptr_t base_address, uint64_t limit_byte ) {
	// file properties in chunks
	uint32_t *vfs = (uint32_t *) base_address;

	// at end of file, magic value exist?
	if( vfs[ (limit_byte >> STD_SHIFT_4) - 1 ] == LIB_VFS_magic ) return TRUE;	// yes

	// no
	return FALSE;
}

struct LIB_VFS_STRUCTURE *kernel_vfs_search_file( struct LIB_VFS_STRUCTURE *directory, uint8_t *name, uint64_t name_length ) {
	// for each data block of directory
	for( uint64_t b = 0; b < (directory -> byte >> STD_SHIFT_PAGE); b++ ) {
		// properties of directory entry
		struct LIB_VFS_STRUCTURE *entry = (struct LIB_VFS_STRUCTURE *) kernel_vfs_block_by_id( directory, b );

		// for every possible entry
		for( uint8_t e = 0; e < STD_PAGE_byte / sizeof( struct LIB_VFS_STRUCTURE ); e++ ) {
			// if 
			if( entry[ e ].name_length == name_length && lib_string_compare( (uint8_t *) entry[ e ].name, name, name_length ) ) return (struct LIB_VFS_STRUCTURE *) &entry[ e ];
		}
	}

	// not located
	return EMPTY;
}

struct LIB_VFS_STRUCTURE *kernel_vfs_path( uint8_t *path, uint64_t length ) {
	// properties of current file
	struct LIB_VFS_STRUCTURE *file;

	// properties of current directory
	struct LIB_VFS_STRUCTURE *directory;

	// start from current file?
	if( *path != STD_ASCII_SLASH ) {
		// properties of task
		struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();
	
		// choose task current file
		directory = task -> directory;
	} else
		// start from default file
		directory = (struct LIB_VFS_STRUCTURE *) kernel -> storage_base_address[ kernel -> storage_root ].device_block;

	// if path is empty
	if( ! length )
		// acquired VFS root file
		return directory;

	// parse path
	while( TRUE ) {
		// remove leading '/', if exist
		while( *path == '/' ) { path++; length--; }

		// select file name from path
		uint64_t name_length = lib_string_word_end( path, length, '/' );

		// locate file inside directory
		file = kernel_vfs_search_file( directory, path, name_length );

		// file not found?
		if( ! file ) return EMPTY;

		// last file from path and requested one?
		if( length == name_length ) {
			// follow symbolic links (if possible)
			while( file -> type & STD_FILE_TYPE_link ) file = (struct LIB_VFS_STRUCTURE *) file -> offset[ FALSE ];

			// acquired file
			return file;
		}

		// preserve current directory
		directory = file;

		// follow symbolic links (if possible)
		while( file -> type & STD_FILE_TYPE_link ) { directory = file; file = (struct LIB_VFS_STRUCTURE *) file -> offset[ FALSE ]; }

		// remove parsed file from path
		path += name_length; length -= name_length;
	}

	// file not found
	return EMPTY;
}

struct KERNEL_STRUCTURE_VFS *kernel_vfs_socket_add( struct LIB_VFS_STRUCTURE *knot ) {
	// available entry, doesn't exist by default
	struct KERNEL_STRUCTURE_VFS *socket = EMPTY;

	// search thru all sockets
	for( uint64_t i = 0; i < KERNEL_VFS_limit; i++ ) {
		// if available for use
		if( ! kernel -> vfs_base_address[ i ].pid ) {
			// properties of socket
			socket = (struct KERNEL_STRUCTURE_VFS *) &kernel -> vfs_base_address[ i ];

			// done
			break;
		}
	}

	// all sockets reserved
	return socket;
}