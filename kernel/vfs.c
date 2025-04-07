/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t kernel_vfs_format( struct KERNEL_STRUCTURE_STORAGE *storage ) {
	// prepare superblock
	struct LIB_VFS_STRUCTURE *superblock = (struct LIB_VFS_STRUCTURE *) kernel -> memory_alloc( TRUE );

	// root directory size in Bytes
	superblock -> limit = STD_PAGE_byte;

	// first block of root directory
	superblock -> block[ FALSE ] = (uintptr_t) kernel -> memory_alloc( TRUE );

	// type
	superblock -> type = STD_FILE_TYPE_directory;

	// name
	superblock -> name_limit = 1;
	superblock -> name[ FALSE ] = STD_ASCII_SLASH;

	// set first block pointer
	storage -> device_block = (uintptr_t) superblock;

	//----------------------------------------------------------------------

	// prepare root directory
	struct LIB_VFS_STRUCTURE *root = (struct LIB_VFS_STRUCTURE *) superblock -> block[ FALSE ];

	// prepare default symlinks for root directory

	// current symlink
	root[ 0 ].block[ FALSE ]	= (uintptr_t) superblock;	// pointing to superblock!
	root[ 0 ].type			= STD_FILE_TYPE_link;
	root[ 0 ].name_limit		= 1;
	root[ 0 ].name[ 0 ]		= STD_ASCII_DOT;

	// previous symlink
	root[ 1 ].block[ FALSE ]	= (uintptr_t) superblock;	// pointing to superblock!
	root[ 1 ].type			= STD_FILE_TYPE_link;
	root[ 1 ].name_limit		= 2;
	root[ 1 ].name[ 0 ]		= STD_ASCII_DOT;
	root[ 1 ].name[ 1 ]		= STD_ASCII_DOT;

	// default size of stoarge
	return STD_PAGE_byte << STD_SHIFT_2;
}

uintptr_t kernel_vfs_block_by_id( struct LIB_VFS_STRUCTURE *vfs, uint64_t i ) {
	// direct block?
	if( ! i-- ) return vfs -> block[ 0 ];	// return pointer

	// indirect block?
	if( i < 512 ) {
		// properties of indirect block
		uintptr_t *indirect = (uintptr_t *) vfs -> block[ 1 ];

		// return pointer
		return indirect[ i ];
	}

	// no support for double-indirect and so on, yet
	return EMPTY;
}

void kernel_vfs_block_fill( struct LIB_VFS_STRUCTURE *vfs, uint64_t i ) {
	// direct blocks

	// block doesn't exist?
	if( ! vfs -> block[ 0 ] ) {	// yep
		// allocate block
		vfs -> block[ 0 ] = kernel_memory_alloc( TRUE );
	}

	// that was last block
	if( ! --i ) return;;

	// indirect block exist?
	if( ! vfs -> block[ 1 ] ) vfs -> block[ 1 ] = kernel_memory_alloc( TRUE );	// no, add

	// indirect blocks
	uintptr_t *indirect = (uintptr_t *) vfs -> block[ 1 ];
	for( uint64_t b = 0; b < 512; b++ ) {
		// that was last block
		if( ! i-- ) return;

		// block exist?
		if( indirect[ b ] ) continue;	// yep

		// allocate block
		indirect[ b ] = kernel_memory_alloc( TRUE );
	}
}

uintptr_t kernel_vfs_block_remove( struct LIB_VFS_STRUCTURE *vfs, uint64_t i ) {
	// removed block pointer
	uintptr_t block = EMPTY;

	// direct block?
	if( ! i ) {
		// preserve block pointer
		block = vfs -> block[ 0 ];

		// remove it
		vfs -> block[ 0 ] = EMPTY;
	}

	// indirect block?
	if( i > 1 && i < 513 ) {
		// properties of indirect block
		uintptr_t *indirect = (uintptr_t *) vfs -> block[ 1 ];

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

struct KERNEL_STRUCTURE_VFS *kernel_vfs_file_open( struct KERNEL_STRUCTURE_STORAGE *storage, uint8_t *path, uint64_t length, uint8_t mode ) {
	// properties of found file
	struct LIB_VFS_STRUCTURE *vfs;
	if( ! (vfs = kernel_vfs_path( path, length )) ) return EMPTY;	// file not found

	// lock exclusive access
	MACRO_LOCK( kernel -> vfs_semaphore );

	// open socket
	struct KERNEL_STRUCTURE_VFS *socket = kernel_vfs_socket_add();

	// file located on definied storage
	socket -> storage = kernel -> storage_root;

	// file identificator
	socket -> knot = (uint64_t) vfs;

	// socket opened by process with ID
	socket -> pid = kernel_task_pid();

	// protect file against any modifications?
	if( mode == STD_FILE_MODE_modify )
		// return flag
		socket -> mode = STD_FILE_MODE_modify;

	// unlock access
	MACRO_UNLOCK( kernel -> vfs_semaphore );

	// file found
	return socket;
}

struct LIB_VFS_STRUCTURE kernel_vfs_file_properties( struct KERNEL_STRUCTURE_VFS *socket ) {
	// properties of file
	return *((struct LIB_VFS_STRUCTURE *) socket -> knot);
}

void kernel_vfs_file_read( struct KERNEL_STRUCTURE_VFS *socket, uint8_t *target, uint64_t seek, uint64_t byte ) {
	// properties of file
	struct LIB_VFS_STRUCTURE *file = (struct LIB_VFS_STRUCTURE *) socket -> knot;

	// invalid read request?
	if( seek + byte > file -> limit ) {
		// read up to end of file?
		if( seek < file -> limit ) byte = file -> limit - seek;
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
	if( ! vfs -> name_limit ) {	// yes
		// set file name
		vfs -> name_limit = EMPTY;
		for( uint8_t j = 0; j < length; j++ ) vfs -> name[ vfs -> name_limit++ ] = name[ j ];

		// set file type
		vfs -> type = type;

		// default block content
		switch( type ) {
			// for file of type: directory
			case STD_FILE_TYPE_directory: {
				// assign first block for directory
				vfs -> block[ FALSE ] = kernel_memory_alloc( TRUE );

				// directory default block properties
				struct LIB_VFS_STRUCTURE *dir = (struct LIB_VFS_STRUCTURE *) vfs -> block[ FALSE ];

				// prepare default symlinks for root directory

				// current location
				dir[ FALSE ].block[ FALSE ]	= (uintptr_t) vfs;
				dir[ FALSE ].type		= STD_FILE_TYPE_link;
				dir[ FALSE ].name_limit	= 1;
				dir[ FALSE ].name[ FALSE ]	= '.';
				
				// previous location
				dir[ TRUE ].block[ FALSE ]	= (uintptr_t) directory;
				dir[ TRUE ].type		= STD_FILE_TYPE_link;
				dir[ TRUE ].name_limit		= 2;
				dir[ TRUE ].name[ FALSE ]	= '.'; dir[ TRUE ].name[ TRUE ] = '.';

				// default directory size
				vfs -> limit = STD_PAGE_byte;

				// done
				break;
			}

			// for file
			default: {
				// clean data pointer
				vfs -> block[ FALSE ] = EMPTY;

				// and file size
				vfs -> limit = EMPTY;
			}
		}
	}

	// open socket
	struct KERNEL_STRUCTURE_VFS *socket = kernel_vfs_socket_add();

	// file located on definied storage
	socket -> storage = kernel -> storage_root;

	// file identificator
	socket -> knot = (uint64_t) vfs;

	// socket opened by process with ID
	socket -> pid = kernel_task_pid();

	// return new socket
	return socket;
}

struct KERNEL_STRUCTURE_VFS *kernel_vfs_touch( struct KERNEL_STRUCTURE_STORAGE *storage, uint8_t *path, uint64_t length, uint8_t type ) {
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
	uint64_t blocks = directory -> limit >> STD_SHIFT_PAGE;
	for( uint64_t b = 0; b < blocks; b++ ) {
		// first directory block entries
		struct LIB_VFS_STRUCTURE *entry = (struct LIB_VFS_STRUCTURE *) kernel_vfs_block_by_id( directory, b );

		// for every possible entry
		for( uint8_t e = 0; e < STD_PAGE_byte / sizeof( struct LIB_VFS_STRUCTURE ); e++ )
			// if free entry, found
			if( ! entry[ e ].name_limit ) return kernel_vfs_file_create( directory, (struct LIB_VFS_STRUCTURE *) &entry[ e ], file_name, file_name_length, type );

		// extend search?
		if( ! kernel_vfs_block_by_id( directory, b + 1 ) ) {
			// expand directory content by block
			kernel_vfs_block_fill( directory, ++blocks );

			// new directory size
			directory -> limit += STD_PAGE_byte;
		}
	}

	// no free entry
	return EMPTY;
}

void kernel_vfs_file_write( struct KERNEL_STRUCTURE_VFS *socket, uint8_t *source, uint64_t seek, uint64_t byte ) {
	// properties of file
	struct LIB_VFS_STRUCTURE *file = (struct LIB_VFS_STRUCTURE *) socket -> knot;

	// insufficient file length?
	if( seek + byte > MACRO_PAGE_ALIGN_UP( file -> limit ) ) {
		// assign required blocks
		kernel_vfs_block_fill( file, MACRO_PAGE_ALIGN_UP( seek + byte ) >> STD_SHIFT_PAGE );
	}

	// set new file length
	file -> limit = seek + byte;

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
	for( uint64_t b = 0; b < (directory -> limit >> STD_SHIFT_PAGE); b++ ) {
		// properties of directory entry
		struct LIB_VFS_STRUCTURE *entry = (struct LIB_VFS_STRUCTURE *) kernel_vfs_block_by_id( directory, b );

		// for every possible entry
		for( uint8_t e = 0; e < STD_PAGE_byte / sizeof( struct LIB_VFS_STRUCTURE ); e++ ) {
			// if 
			if( entry[ e ].name_limit == name_length && lib_string_compare( (uint8_t *) entry[ e ].name, name, name_length ) ) return (struct LIB_VFS_STRUCTURE *) &entry[ e ];
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

	// start from directory file?
	if( *path != STD_ASCII_SLASH ) {
		// properties of task
		struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();
	
		// choose task current file
		directory = (struct LIB_VFS_STRUCTURE *) task -> directory;
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
			while( file -> type & STD_FILE_TYPE_link ) file = (struct LIB_VFS_STRUCTURE *) file -> block[ FALSE ];

			// acquired file
			return file;
		}

		// preserve current directory
		directory = file;

		// follow symbolic links (if possible)
		while( file -> type & STD_FILE_TYPE_link ) { directory = file; file = (struct LIB_VFS_STRUCTURE *) file -> block[ FALSE ]; }

		// remove parsed file from path
		path += name_length; length -= name_length;
	}

	// file not found
	return EMPTY;
}

struct KERNEL_STRUCTURE_VFS *kernel_vfs_socket_add( void ) {
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

void kernel_vfs_read( uint64_t reserved, uint64_t block, uint8_t *target, uint64_t length ) {
	// read Bytes
	while( length-- ) *(target++) = *((uint8_t *) block++);
}

void kernel_vfs_write( uint64_t reserved, uint64_t block, uint8_t *source, uint64_t length ) {
	// write Bytes
	while( length-- ) *((uint8_t *) block++) = *(source++);
}

uintptr_t kernel_vfs_dir( uint64_t storage_id, uint8_t *path, uint64_t length ) {
	// properties of selected directory
	struct LIB_VFS_STRUCTURE *directory;
	if( ! (directory = kernel_vfs_path( path, lib_string_length( path ) )) ) return EMPTY;	// doesn't exist

	// it is directory?
	if( directory -> type != STD_FILE_TYPE_directory ) return EMPTY;	// no

	// amount of available files
	uint64_t file_count = EMPTY;

	// for each data block of directory
	for( uint64_t b = 0; b < (directory -> limit >> STD_SHIFT_PAGE); b++ ) {
		// properties of directory entry
		struct LIB_VFS_STRUCTURE *file = (struct LIB_VFS_STRUCTURE *) kernel_vfs_block_by_id( directory, b );

		// for every possible entry
		for( uint8_t e = 0; e < STD_PAGE_byte / sizeof( struct LIB_VFS_STRUCTURE ); e++ ) if( file[ e ].name_limit ) file_count++;
	}

	// alloc area for presented files
	struct LIB_VFS_STRUCTURE *file = (struct LIB_VFS_STRUCTURE *) kernel_syscall_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct LIB_VFS_STRUCTURE ) * (file_count + 1) ) >> STD_SHIFT_PAGE );

	// index of shared file
	uint64_t file_index = EMPTY;

	// share file properties
	for( uint64_t b = 0; b < (directory -> limit >> STD_SHIFT_PAGE); b++ ) {
		// properties of directory entry
		struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) kernel_vfs_block_by_id( directory, b );

		// for every possible entry
		for( uint8_t e = 0; e < STD_PAGE_byte / sizeof( struct LIB_VFS_STRUCTURE ); e++ ) if( vfs[ e ].name_limit ) file[ file_index++ ] = vfs[ e ];
	}

	// return list of files inside directory
	return (uintptr_t) file;
}

struct LIB_VFS_STRUCTURE kernel_vfs_file( struct KERNEL_STRUCTURE_STORAGE *storage, uint64_t file_id ) {
	// properties of file
	struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) file_id;

	// return properties of file
	return *vfs;
}