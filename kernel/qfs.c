/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// debug, memory
// for( uint8_t i = 0; i < 8; i ++ ) { kernel -> log( (uint8_t *) "0x%16X", (uintptr_t) block_data + (16 * i) ); for( uint8_t j = 0; j < 16; j++ ) kernel -> log( (uint8_t *) " %2X", block_data[ (16 * i) + j ] ); kernel -> log( (uint8_t *) "\n" ); }

uint64_t kernel_qfs_acquire( uint32_t *memory_map, uint64_t N, uint64_t p, uint64_t limit ) {
	// search binary map for N continuous blocks
	for( ; (p + N) < limit; p++ ) {
		// by default we found N enabled bits
		uint8_t found = TRUE;

		// check N (c)onsecutive blocks
		for( uint64_t c = p; c < (p + N); c++ ) {
			// continous?
			if( memory_map[ c >> STD_SHIFT_32 ] & 1 << (c & 0b00011111) ) continue;

			// one of the bits is disabled
			found = FALSE;

			// start looking from next position
			p = c;

			// restart
			break;
		}

		// if N consecutive blocks have been found
		if( ! found ) continue;	// nope

		// mark pages as (r)eserved
		for( uint64_t r = p; r < (p + N); r++ )
			memory_map[ r >> STD_SHIFT_32 ] &= ~(1 << (r & 0b00011111) );

		// return first block of acquired set
		return p;
	}

	// no available blocks
	return EMPTY;
}

uint64_t kernel_qfs_alloc( struct KERNEL_STRUCTURE_STORAGE *storage ) {
	// properties of superblock
	struct LIB_VFS_STRUCTURE *superblock = (struct LIB_VFS_STRUCTURE *) kernel -> memory_alloc( TRUE );
	storage -> block_read( storage -> device_id, storage -> device_block, (uint8_t *) superblock, LIB_VFS_BLOCK_byte / storage -> device_byte );

	// load bitmap of current file system storage
	uint32_t *bitmap = (uint32_t *) kernel -> memory_alloc( superblock -> block[ FALSE ] - TRUE );
	storage -> block_read( storage -> device_id, storage -> device_block + (LIB_VFS_BLOCK_byte / storage -> device_byte), (uint8_t *) bitmap, (LIB_VFS_BLOCK_byte / storage -> device_byte) * (superblock -> block[ FALSE ] - TRUE) );

	// alloc block
	uint64_t block_id = kernel_qfs_acquire( bitmap, TRUE, EMPTY, 16 );

	// update bitmap of current file system storage
	storage -> block_write( storage -> device_id, storage -> device_block + (LIB_VFS_BLOCK_byte / storage -> device_byte), (uint8_t *) bitmap, (LIB_VFS_BLOCK_byte / storage -> device_byte) * (superblock -> block[ FALSE ] - TRUE) );

	// release bitmap area
	kernel -> memory_release( (uintptr_t) bitmap, superblock -> block[ FALSE ] - TRUE );

	// release superblock area
	kernel -> memory_release( (uintptr_t) superblock, TRUE );

	// return allocated block
	return block_id;
}

uintptr_t kernel_qfs_block( struct KERNEL_STRUCTURE_STORAGE *storage, struct LIB_VFS_STRUCTURE *vfs, uint64_t block_id ) {
	// alloc area for block data
	uint64_t *block_data = (uint64_t *) kernel -> memory_alloc( TRUE );

	// direct block?
	if( ! block_id-- ) storage -> block_read( storage -> device_id, storage -> device_block + (vfs -> block[ FALSE ] * (LIB_VFS_BLOCK_byte / storage -> device_byte)), (uint8_t *) block_data, LIB_VFS_BLOCK_byte / storage -> device_byte );

	// indirect block?
	else if( block_id < 512 ) {
		// load content of indirect block
		storage -> block_read( storage -> device_id, storage -> device_block + (vfs -> block[ TRUE ] * (LIB_VFS_BLOCK_byte / storage -> device_byte)), (uint8_t *) block_data, LIB_VFS_BLOCK_byte / storage -> device_byte );

		// properties of indirect block
		storage -> block_read( storage -> device_id, storage -> device_block + (block_data[ block_id ] * (LIB_VFS_BLOCK_byte / storage -> device_byte)), (uint8_t *) block_data, LIB_VFS_BLOCK_byte / storage -> device_byte );
	}

	// todo, support for double-indirect and so on
	return (uintptr_t) block_data;
}

uint64_t kernel_qfs_block_id( struct KERNEL_STRUCTURE_STORAGE *storage, struct LIB_VFS_STRUCTURE *vfs, uint64_t block_id ) {
	// physical block id, by default first one
	uint64_t id = vfs -> block[ FALSE ];

	// direct block?
	if( ! block_id-- ) return id;	// obtained

	// indirect block?
	else if( block_id < 512 ) {
		// alloc area for block data
		uint64_t *block_data = (uint64_t *) kernel -> memory_alloc( TRUE );

		// load content of indirect block
		storage -> block_read( storage -> device_id, storage -> device_block + (vfs -> block[ TRUE ] * (LIB_VFS_BLOCK_byte / storage -> device_byte)), (uint8_t *) block_data, LIB_VFS_BLOCK_byte / storage -> device_byte );

		// physical block id
		id = block_data[ block_id ];

		// release data block
		kernel -> memory_release( (uintptr_t) block_data, TRUE );
	}

	// todo, support for double-indirect and so on
	return id;
}

void kernel_qfs_format( struct KERNEL_STRUCTURE_STORAGE *storage ) {
	// prepare superblock
	struct LIB_VFS_STRUCTURE *superblock = (struct LIB_VFS_STRUCTURE *) kernel -> memory_alloc( TRUE );

	// root directory size in Bytes
	superblock -> limit = STD_PAGE_byte;

	// bitmap length in blocks
	uint64_t bitmap_block = ((MACRO_PAGE_ALIGN_DOWN( storage -> device_byte * storage -> device_limit ) >> STD_SHIFT_PAGE) - (TRUE + TRUE)) / 32768;
	if( ((MACRO_PAGE_ALIGN_DOWN( storage -> device_byte * storage -> device_limit ) >> STD_SHIFT_PAGE) - (TRUE + TRUE)) % 32768 ) bitmap_block++;

	// first block of root directory
	superblock -> block[ FALSE ] = TRUE + bitmap_block;

	// available space
	storage -> device_free = MACRO_PAGE_ALIGN_DOWN( storage -> device_byte * storage -> device_limit ) - ((TRUE + bitmap_block + TRUE) << STD_SHIFT_PAGE);

	// type
	superblock -> type = STD_FILE_TYPE_directory;

	// name
	superblock -> name_limit = 1;
	superblock -> name[ FALSE ] = STD_ASCII_SLASH;

	// write inside first usable block of storage
	storage -> block_write( storage -> device_id, storage -> device_block, (uint8_t *) superblock, LIB_VFS_BLOCK_byte / storage -> device_byte );

	// release superblock content
	kernel -> memory_release( (uintptr_t) superblock, TRUE );

	//----------------------------------------------------------------------

	// prepare bitmap
	uint32_t *bitmap = (uint32_t *) kernel -> memory_alloc( bitmap_block );

	// clean'up
	for( uint64_t i = 0; i < (MACRO_PAGE_ALIGN_DOWN( storage -> device_byte * storage -> device_limit ) >> STD_SHIFT_PAGE) << STD_SHIFT_8; i++ ) bitmap[ i >> STD_SHIFT_32 ] |= 1 << (i & 0b00011111);

	// mark pages used by file system structures as unusable
	for( uint64_t i = 0; i < (TRUE + bitmap_block + TRUE); i++ ) bitmap[ i >> STD_SHIFT_32 ] &= ~(1 << (i & 0b00011111) );

	// write after superblock
	storage -> block_write( storage -> device_id, storage -> device_block + (LIB_VFS_BLOCK_byte / 512), (uint8_t *) bitmap, (bitmap_block << STD_SHIFT_PAGE) / storage -> device_byte );

	// release bitmap content
	kernel -> memory_release( (uintptr_t) bitmap, bitmap_block );

	//----------------------------------------------------------------------

	// prepare root directory
	struct LIB_VFS_STRUCTURE *root = (struct LIB_VFS_STRUCTURE *) kernel -> memory_alloc( TRUE );

	// prepare default symlinks for root directory

	// current symlink
	root[ 0 ].block[ FALSE ]	= EMPTY;	// pointing to superblock!
	root[ 0 ].type			= STD_FILE_TYPE_link;
	root[ 0 ].name_limit		= 1;
	root[ 0 ].name[ 0 ]		= STD_ASCII_DOT;

	// previous symlink
	root[ 1 ].block[ FALSE ]	= EMPTY;	// pointing to superblock!
	root[ 1 ].type			= STD_FILE_TYPE_link;
	root[ 1 ].name_limit		= 2;
	root[ 1 ].name[ 0 ]		= STD_ASCII_DOT;
	root[ 1 ].name[ 1 ]		= STD_ASCII_DOT;

	// write after bitmap
	storage -> block_write( storage -> device_id, storage -> device_block + ((TRUE + bitmap_block) * 8), (uint8_t *) root, LIB_VFS_BLOCK_byte / storage -> device_byte );

	// release root directory content
	kernel -> memory_release( (uintptr_t) root, TRUE );
}

uint8_t kernel_qfs_identify( struct KERNEL_STRUCTURE_STORAGE *storage ) {
	// lets assume, that is Quark File system
	uint8_t qfs = TRUE;

	// properties of knot under consideration
	struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) kernel -> memory_alloc( TRUE );

	// load superblock knot
	storage -> block_read( storage -> device_id, storage -> device_block, (uint8_t *) vfs, LIB_VFS_BLOCK_byte / storage -> device_byte );

	// knot type of directory?
	if( ! (vfs -> type & STD_FILE_TYPE_directory ) ) qfs = FALSE;	// no

	// knot name length is correct?
	if( vfs -> name_limit != TRUE ) qfs = FALSE;	// no

	// knot name is correct?
	if( vfs -> name[ FALSE ] != STD_ASCII_SLASH ) qfs = FALSE;	// no

	// first block of knot is within limits of storage?
	if( (vfs -> block[ FALSE ] * (LIB_VFS_BLOCK_byte / storage -> device_byte)) >= storage -> device_limit ) qfs = FALSE;	// no

	// continue?
	if( qfs ) {
		// load first block of root directory content
		storage -> block_read( storage -> device_id, storage -> device_block + (vfs -> block[ FALSE ] * (LIB_VFS_BLOCK_byte / storage -> device_byte)), (uint8_t *) vfs, LIB_VFS_BLOCK_byte / storage -> device_byte );

		// knot type of link?
		if( ! (vfs -> type & STD_FILE_TYPE_link ) ) qfs = FALSE;	// no

		// knot name length is correct?
		if( vfs -> name_limit != TRUE ) qfs = FALSE;	// no

		// knot name is correct?
		if( vfs -> name[ FALSE ] != STD_ASCII_SLASH ) qfs = FALSE;	// no

		// link leads to superblock?
		if( ! vfs -> block[ FALSE ] ) qfs = FALSE;	// no
	}

	// release knot
	kernel -> memory_release( (uintptr_t) vfs, TRUE );

	// it is?
	return qfs;
}

struct LIB_VFS_STRUCTURE kernel_qfs_properties_by_socket( struct KERNEL_STRUCTURE_VFS *socket ) {
	// properties of file
	return kernel_qfs_file( (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ socket -> storage ], socket -> knot );
}

struct LIB_VFS_STRUCTURE kernel_qfs_file( struct KERNEL_STRUCTURE_STORAGE *storage, uint64_t file_id ) {
	// allocate block for data
	uint8_t *block_data = (uint8_t *) kernel -> memory_alloc( TRUE );

	// load block of data containing knot
	storage -> block_read( storage -> device_id, storage -> device_block + (((file_id & STD_PAGE_mask) >> STD_SHIFT_PAGE) * (LIB_VFS_BLOCK_byte / storage -> device_byte)), block_data, LIB_VFS_BLOCK_byte / storage -> device_byte );

	// properties of file
	struct LIB_VFS_STRUCTURE vfs = *((struct LIB_VFS_STRUCTURE *) &block_data[ (file_id & ~STD_PAGE_mask) * sizeof( struct LIB_VFS_STRUCTURE ) ]);

	// release block data
	kernel -> memory_release( (uintptr_t) block_data, TRUE );

	// return file properties
	return vfs;
}

void kernel_qfs_file_update( struct KERNEL_STRUCTURE_STORAGE *storage, struct LIB_VFS_STRUCTURE *file, uint64_t file_id ) {
	// allocate block for data
	uint8_t *block_data = (uint8_t *) kernel -> memory_alloc( TRUE );

	// load block of data containing knot
	storage -> block_read( storage -> device_id, storage -> device_block + (((file_id & STD_PAGE_mask) >> STD_SHIFT_PAGE) * (LIB_VFS_BLOCK_byte / storage -> device_byte)), block_data, LIB_VFS_BLOCK_byte / storage -> device_byte );

	// for( uint8_t i = 0; i < 96; i ++ ) { kernel -> log( (uint8_t *) "0x%16X", (uintptr_t) block_data + (16 * i) ); for( uint8_t j = 0; j < 16; j++ ) kernel -> log( (uint8_t *) " %2X", block_data[ (16 * i) + j ] ); kernel -> log( (uint8_t *) "\n" ); }

	// properties of file
	*((struct LIB_VFS_STRUCTURE *) &block_data[ (file_id & ~STD_PAGE_mask) * sizeof( struct LIB_VFS_STRUCTURE ) ]) = *file;

	// update block of data containing knot
	storage -> block_write( storage -> device_id, storage -> device_block + (((file_id & STD_PAGE_mask) >> STD_SHIFT_PAGE) * (LIB_VFS_BLOCK_byte / storage -> device_byte)), block_data, LIB_VFS_BLOCK_byte / storage -> device_byte );

	// release block data
	kernel -> memory_release( (uintptr_t) block_data, TRUE );
}

uintptr_t kernel_qfs_dir( struct KERNEL_STRUCTURE_STORAGE *storage, uint8_t *path, uint64_t length ) {
	// properties of task
	struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();

	// properties of selected directory
	uint64_t directory_id = kernel_qfs_path( storage, path, lib_string_length( path ) );
	struct LIB_VFS_STRUCTURE directory = kernel_qfs_file( storage, directory_id );

	// it is directory?
	if( directory.type != STD_FILE_TYPE_directory ) return EMPTY;	// no

	// amount of available files
	uint64_t file_count = EMPTY;

	// for each data block of directory
	for( uint64_t b = 0; b < (directory.limit >> STD_SHIFT_PAGE); b++ ) {
		// properties of directory entry
		struct LIB_VFS_STRUCTURE *file = (struct LIB_VFS_STRUCTURE *) kernel_qfs_block( storage, (struct LIB_VFS_STRUCTURE *) &directory, b );

		// for every possible entry
		for( uint8_t e = 0; e < STD_PAGE_byte / sizeof( struct LIB_VFS_STRUCTURE ); e++ ) if( file[ e ].name_limit ) file_count++;

		// release data block
		kernel -> memory_release( (uintptr_t) file, TRUE );
	}

	// alloc area for presented files
	struct LIB_VFS_STRUCTURE *file = (struct LIB_VFS_STRUCTURE *) kernel_syscall_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct LIB_VFS_STRUCTURE ) * (file_count + 1) ) >> STD_SHIFT_PAGE );

	// index of shared file
	uint64_t file_index = EMPTY;

	// share file properties
	for( uint64_t b = 0; b < (directory.limit >> STD_SHIFT_PAGE); b++ ) {
		// properties of directory entry
		struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) kernel_qfs_block( storage, (struct LIB_VFS_STRUCTURE *) &directory, b );

		// for every possible entry
		for( uint8_t e = 0; e < STD_PAGE_byte / sizeof( struct LIB_VFS_STRUCTURE ); e++ ) if( vfs[ e ].name_limit ) file[ file_index++ ] = vfs[ e ];

		// release data block
		kernel -> memory_release( (uintptr_t) vfs, TRUE );
	}

	// return list of files inside directory
	return (uintptr_t) file;
}

uint64_t kernel_qfs_path( struct KERNEL_STRUCTURE_STORAGE *storage, uint8_t *path, uint64_t length ) {
	// properties of task
	struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();

	// properties of current directory
	uint64_t directory_id = task -> directory;

	// start from root directory?
	if( *path == STD_ASCII_SLASH ) directory_id = storage -> fs.root_directory_id;	// yes

	// parse path
	while( TRUE ) {
		// remove leading '/', if exist
		while( *path == '/' ) { path++; length--; }

		// end of path?
		if( ! length ) return directory_id;	// found directory

		// select file name from path
		uint64_t name_length = lib_string_word_end( path, length, '/' );

		// locate file inside directory
		struct LIB_VFS_STRUCTURE directory = kernel_qfs_file( storage, directory_id );
		uint64_t file_id = kernel_qfs_search( storage, (struct LIB_VFS_STRUCTURE *) &directory, path, name_length );

		// file not found?
		if( ! file_id ) return EMPTY;

		// last file from path and requested one?
		if( length == name_length ) {
			// follow symbolic links (if possible)
			struct LIB_VFS_STRUCTURE file = kernel_qfs_file( storage, file_id );
			while( file.type == STD_FILE_TYPE_link ) { file_id = file.block[ FALSE ]; file = kernel_qfs_file( storage, file_id ); }

			// return file identificator
			return file_id;
		}
	}

	// file not found
	return EMPTY;
}

uint64_t kernel_qfs_search( struct KERNEL_STRUCTURE_STORAGE *storage, struct LIB_VFS_STRUCTURE *directory, uint8_t *name, uint64_t name_length ) {
	// located file id
	uint64_t id = EMPTY;

	// share file properties
	for( uint64_t b = 0; b < (directory -> limit >> STD_SHIFT_PAGE); b++ ) {
		// properties of directory entry
		struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) kernel_qfs_block( storage, directory, b );

		// for every possible entry
		for( uint8_t e = 0; e < STD_PAGE_byte / sizeof( struct LIB_VFS_STRUCTURE ); e++ ) {
			// file located?
			if( vfs[ e ].name_limit == name_length && lib_string_compare( (uint8_t *) vfs[ e ].name, name, name_length ) ) {	// yes
				// calculate file id
				id = (kernel_qfs_block_id( storage, directory, b ) << STD_SHIFT_PAGE) + e;

				// ignore further search
				break;
			}
		}

		// release data block
		kernel -> memory_release( (uintptr_t) vfs, TRUE );

		// if file located
		if( id ) break;	// ignore further search
	}

	// not located
	return id;
}

uint64_t kernel_qfs_touch( struct KERNEL_STRUCTURE_STORAGE *storage, uint8_t *path, uint64_t length, uint8_t type ) {
	// pointer to last file name inside path
	uint8_t *file_name = lib_string_basename( path );

	// file name length
	uint64_t file_name_length = length - ((uintptr_t) file_name - (uintptr_t) path);

	// open destination directory
	uint64_t directory_id = kernel_qfs_path( storage, path, length - file_name_length );
	struct LIB_VFS_STRUCTURE directory = kernel_qfs_file( storage, directory_id );

	// check if file already exist
	uint64_t file_id = kernel_qfs_search( storage, (struct LIB_VFS_STRUCTURE *) &directory, file_name, file_name_length );
	if( file_id ) return file_id;

	// for each data block of directory
	uint64_t blocks = directory.limit >> STD_SHIFT_PAGE;
	for( uint64_t b = 0; b < blocks; b++ ) {
		// properties of directory entry
		struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) kernel_qfs_block( storage, (struct LIB_VFS_STRUCTURE *) &directory, b );

		// for every possible entry
		for( uint8_t e = 0; e < STD_PAGE_byte / sizeof( struct LIB_VFS_STRUCTURE ); e++ )
			// if free entry, found
			if( ! vfs[ e ].name_limit ) {
				// create new entry
				kernel_qfs_create( storage, directory_id, (kernel_qfs_block_id( storage, (struct LIB_VFS_STRUCTURE *) &directory, b ) << STD_SHIFT_PAGE) + e, (struct LIB_VFS_STRUCTURE *) &vfs[ e ], file_name, file_name_length, type );

				// update block content
				storage -> block_write( storage -> device_id, storage -> device_block + (kernel_qfs_block_id( storage, (struct LIB_VFS_STRUCTURE *) &directory, b ) * (LIB_VFS_BLOCK_byte / storage -> device_byte)), (uint8_t *) vfs, LIB_VFS_BLOCK_byte / storage -> device_byte );

				// return properties of file
				file_id = (kernel_qfs_block_id( storage, (struct LIB_VFS_STRUCTURE *) &directory, b ) << STD_SHIFT_PAGE) + e;

				// file created, block updated
				break;
			}

		// release data block
		kernel -> memory_release( (uintptr_t) vfs, TRUE );

		// if file created
		if( file_id ) return file_id;	// ignore further search

		// extend search?
		if( ! kernel_qfs_block_id( storage, (struct LIB_VFS_STRUCTURE *) &directory, b + 1 ) ) {
			// expand directory content by block
			kernel_qfs_block_fill( storage, (struct LIB_VFS_STRUCTURE *) &directory, ++blocks );

			// new directory size
			directory.limit += STD_PAGE_byte;
		}
	}

	// no free entry
	return EMPTY;
}

void kernel_qfs_create( struct KERNEL_STRUCTURE_STORAGE *storage, uint64_t directory_id, uint64_t file_id, struct LIB_VFS_STRUCTURE *vfs, uint8_t *name, uint64_t limit, uint8_t type ) {
	// set file type
	vfs -> type = type;

	// set file name and limit
	vfs -> name_limit = EMPTY;
	for( uint8_t j = 0; j < limit; j++ ) vfs -> name[ vfs -> name_limit++ ] = name[ j ];

	// default block content
	switch( type ) {
		// for file of type: directory
		case STD_FILE_TYPE_directory: {
			// assign first block for directory
			vfs -> block[ FALSE ] = kernel_qfs_alloc( storage );

			// prepare default symlinks for directory
			struct LIB_VFS_STRUCTURE *dir = (struct LIB_VFS_STRUCTURE *) kernel_qfs_block( storage, vfs, FALSE );

			kernel_memory_clean( (uint64_t *) dir, 1 );

			// current symlink
			dir[ 0 ].block[ FALSE ]	= file_id;	// pointing to itself
			dir[ 0 ].type		= STD_FILE_TYPE_link;
			dir[ 0 ].name_limit	= 1;
			dir[ 0 ].name[ 0 ]	= STD_ASCII_DOT;

			// previous symlink
			dir[ 1 ].block[ FALSE ]	= directory_id;	// parent directory
			dir[ 1 ].type		= STD_FILE_TYPE_link;
			dir[ 1 ].name_limit	= 2;
			dir[ 1 ].name[ 0 ]	= STD_ASCII_DOT;
			dir[ 1 ].name[ 1 ]	= STD_ASCII_DOT;

			// default directory size
			vfs -> limit = STD_PAGE_byte;

			// update block content
			storage -> block_write( storage -> device_id, storage -> device_block + (vfs -> block[ FALSE ] * (LIB_VFS_BLOCK_byte / storage -> device_byte)), (uint8_t *) dir, LIB_VFS_BLOCK_byte / storage -> device_byte );

			// release data block
			kernel -> memory_release( (uintptr_t) dir, TRUE );

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

struct KERNEL_STRUCTURE_VFS *kernel_qfs_open( struct KERNEL_STRUCTURE_STORAGE *storage, uint8_t *path, uint64_t length, uint8_t mode ) {
	// id of found file
	uint64_t file_id = kernel_qfs_path( storage, path, length );

	// lock exclusive access
	MACRO_LOCK( kernel -> vfs_semaphore );

	// open socket
	struct KERNEL_STRUCTURE_VFS *socket = kernel_vfs_socket_add();

	// properties of task
	struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();

	// file located on definied storage
	socket -> storage = ((uintptr_t) storage - (uintptr_t) kernel -> storage_base_address) / sizeof( struct KERNEL_STRUCTURE_STORAGE );

	// file identificator
	socket -> knot = file_id;

	// socket opened by process with ID
	socket -> pid = task -> pid;

	// protect file against any modifications?
	if( mode == STD_FILE_MODE_modify )
		// return flag
		socket -> mode = STD_FILE_MODE_modify;

	// unlock access
	MACRO_UNLOCK( kernel -> vfs_semaphore );

	// file found
	return socket;
}

void kernel_qfs_close( struct KERNEL_STRUCTURE_VFS *socket ) {
	// can we close file?
	if( socket -> pid != kernel_task_pid() ) return;	// no! TODO: something nasty

	// release socket
	socket -> pid = EMPTY;
}

void kernel_qfs_block_fill( struct KERNEL_STRUCTURE_STORAGE *storage, struct LIB_VFS_STRUCTURE *vfs, uint64_t i ) {
	// direct blocks

	// block doesn't exist?
	if( ! vfs -> block[ 0 ] ) vfs -> block[ 0 ] = kernel_qfs_alloc( storage );	// no, add

	// that was last block
	if( ! --i ) return;	// yes

	// indirect block exist?
	if( ! vfs -> block[ 1 ] ) vfs -> block[ 1 ] = kernel_qfs_alloc( storage );	// no, add

	// read indirect block content
	uintptr_t *indirect = (uintptr_t *) kernel_qfs_block( storage, vfs, vfs -> block[ 1 ] );

	// indirect blocks
	for( uint64_t b = 0; b < 512; b++ ) {
		// that was last block
		if( ! i-- ) break;

		// block exist?
		if( indirect[ b ] ) continue;	// yep

		// allocate block
		indirect[ b ] = kernel_qfs_alloc( storage );
	}

	// write new indirect block content
	storage -> block_write( storage -> device_id, storage -> device_block + (vfs -> block[ 1 ] * (LIB_VFS_BLOCK_byte / storage -> device_byte)), (uint8_t *) indirect, LIB_VFS_BLOCK_byte / storage -> device_byte );

	// release content of indirect block
	kernel -> memory_release( (uintptr_t) indirect, TRUE );
}

void kernel_qfs_write( struct KERNEL_STRUCTURE_VFS *socket, uint8_t *source, uint64_t seek, uint64_t byte ) {
	// properties of storage
	struct KERNEL_STRUCTURE_STORAGE *storage = (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ socket -> storage ];

	// properties of file
	struct LIB_VFS_STRUCTURE file = kernel_qfs_file( storage, socket -> knot );

	// insufficient file length?
	if( seek + byte > MACRO_PAGE_ALIGN_UP( file.limit ) ) kernel_qfs_block_fill( storage, (struct LIB_VFS_STRUCTURE *) &file, MACRO_PAGE_ALIGN_UP( seek + byte ) >> STD_SHIFT_PAGE );	// no

	// set new file length
	file.limit = seek + byte;

	// INFO: writing to file from seek == EMPTY, means the same as create new content

	// calculate first block number
	uint64_t b = (MACRO_PAGE_ALIGN_DOWN( seek ) >> STD_SHIFT_PAGE);

	// write first part of data
	seek %= STD_PAGE_byte;

	// target block pointer
	uint8_t *target = (uint8_t *) kernel -> memory_alloc( TRUE );

	// write all blocks with provided data
	while( byte ) {
		// load block of data containing knot
		storage -> block_read( storage -> device_id, storage -> device_block + (kernel_qfs_block_id( storage, (struct LIB_VFS_STRUCTURE *) &file, b ) * (LIB_VFS_BLOCK_byte / storage -> device_byte)), target, LIB_VFS_BLOCK_byte / storage -> device_byte );

		// full or part of block?
		uint64_t limit = STD_PAGE_byte;
		if( limit > byte ) limit = byte;

		// copy data to block
		for( uint64_t i = seek; i < limit && byte--; i++ ) target[ i ] = *(source++);

		// update block of data containing knot
		storage -> block_write( storage -> device_id, storage -> device_block + (kernel_qfs_block_id( storage, (struct LIB_VFS_STRUCTURE *) &file, b++ ) * (LIB_VFS_BLOCK_byte / storage -> device_byte)), target, LIB_VFS_BLOCK_byte / storage -> device_byte );

		// start from begining of next block
		seek = EMPTY;
	}

	// release block content
	kernel -> memory_release( (uintptr_t) target, TRUE );

	// // truncate no more needed file blocks
	// do {
	// 	// obtain block to truncate
	// 	uintptr_t block = kernel_qfs_block_remove( (struct LIB_VFS_STRUCTURE *) &file, b );

	// 	// no more blocks?
	// 	if( ! block ) break;	// yes

	// 	// release it
	// 	kernel_memory_release( block, TRUE );
	// // until forever
	// } while( TRUE );

	// update properties of file
	kernel_qfs_file_update( storage, (struct LIB_VFS_STRUCTURE *) &file, socket -> knot );
}

void kernel_qfs_read( struct KERNEL_STRUCTURE_VFS *socket, uint8_t *target, uint64_t seek, uint64_t byte ) {
	// properties of storage
	struct KERNEL_STRUCTURE_STORAGE *storage = (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ socket -> storage ];

	// properties of file
	struct LIB_VFS_STRUCTURE file = kernel_qfs_file( storage, socket -> knot );

	// invalid read request?
	if( seek + byte > file.limit ) {
		// read up to end of file?
		if( seek < file.limit ) byte = file.limit - seek;	// yes
		else return;	// no, ignore
	}

	// calculate first block number
	uint64_t b = (MACRO_PAGE_ALIGN_DOWN( seek ) >> STD_SHIFT_PAGE);

	// read first part of file
	seek %= STD_PAGE_byte;

	// source block pointer
	uint8_t *source = (uint8_t *) kernel -> memory_alloc( TRUE );

	// read all blocks containing requested data
	while( byte ) {
		// load block of data containing knot
		storage -> block_read( storage -> device_id, storage -> device_block + (kernel_qfs_block_id( storage, (struct LIB_VFS_STRUCTURE *) &file, b ) * (LIB_VFS_BLOCK_byte / storage -> device_byte)), source, LIB_VFS_BLOCK_byte / storage -> device_byte );

		// full or part of block?
		uint64_t limit = STD_PAGE_byte;
		if( limit > byte ) limit = byte;

		// copy data to block
		for( uint64_t i = seek; i < limit && byte--; i++ ) *(target++) = source[ i ];

		// start from begining of next block
		seek = EMPTY;
	}

	// release block content
	kernel -> memory_release( (uintptr_t) source, TRUE );
}