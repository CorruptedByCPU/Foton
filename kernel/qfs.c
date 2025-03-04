/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// debug, memory
// for( uint8_t i = 0; i < 8; i ++ ) { kernel -> log( (uint8_t *) "0x%16X", (uintptr_t) block_data + (16 * i) ); for( uint8_t j = 0; j < 16; j++ ) kernel -> log( (uint8_t *) " %2X", block_data[ (16 * i) + j ] ); kernel -> log( (uint8_t *) "\n" ); }

uint64_t kernel_qfs_alloc( struct KERNEL_STRUCTURE_STORAGE *storage ) {
	// properties of superblock
	struct LIB_VFS_STRUCTURE *superblock = (struct LIB_VFS_STRUCTURE *) kernel -> memory_alloc( TRUE );
	storage -> block_read( storage -> device_id, storage -> device_block, (uint8_t *) superblock, LIB_VFS_BLOCK_byte / storage -> device_byte );

	// load bitmap of current file system storage
	uint32_t *bitmap = (uint32_t *) kernel -> memory_alloc( superblock -> block[ FALSE ] - TRUE );
	storage -> block_read( storage -> device_id, storage -> device_block + (LIB_VFS_BLOCK_byte / storage -> device_byte), (uint8_t *) bitmap, (LIB_VFS_BLOCK_byte / storage -> device_byte) * (superblock -> block[ FALSE ] - TRUE) );

	// alloc block
	uint64_t block_id = kernel_memory_acquire( bitmap, TRUE, EMPTY, 16 );

	// update bitmap of current file system storage
	storage -> block_write( storage -> device_id, storage -> device_block + (LIB_VFS_BLOCK_byte / storage -> device_byte), (uint8_t *) bitmap, (LIB_VFS_BLOCK_byte / storage -> device_byte) * (superblock -> block[ FALSE ] - TRUE) );

	// return allocated block
	return block_id;
}

uintptr_t kernel_qfs_block( struct KERNEL_STRUCTURE_STORAGE *storage, struct LIB_VFS_STRUCTURE *vfs, uint64_t block_id ) {
	// alloc area for block data
	uint64_t *block_data = (uint64_t *) kernel -> memory_alloc( TRUE );

	// direct block?
	if( ! block_id-- ) storage -> block_read( storage -> device_id, (vfs -> block[ FALSE ] * (LIB_VFS_BLOCK_byte / storage -> device_byte)), (uint8_t *) block_data, LIB_VFS_BLOCK_byte / storage -> device_byte );

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
	for( uint64_t i = 0; i < bitmap_block << STD_SHIFT_1024; i++ ) bitmap[ i ] = STD_MAX_unsigned;

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

struct LIB_VFS_STRUCTURE kernel_qfs_file( struct KERNEL_STRUCTURE_STORAGE *storage, uint64_t file_id ) {
	// allocate block for data
	uint8_t *block_data = (uint8_t *) kernel -> memory_alloc( TRUE );

	// load block of data containing knot
	storage -> block_read( storage -> device_id, storage -> device_block + (((file_id & STD_PAGE_mask) >> STD_SHIFT_PAGE) * (LIB_VFS_BLOCK_byte / storage -> device_byte)), block_data, LIB_VFS_BLOCK_byte / storage -> device_byte );

	// properties of file
	struct LIB_VFS_STRUCTURE vfs = *((struct LIB_VFS_STRUCTURE *) &block_data[ file_id & ~STD_PAGE_mask ]);

	// release block data
	kernel -> memory_release( (uintptr_t) block_data, TRUE );

	// return file properties
	return vfs;
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

// debug
for( uint8_t i = 0; i < 8; i ++ ) { kernel -> log( (uint8_t *) "0x%16X", (uintptr_t) vfs + (16 * i) ); for( uint8_t j = 0; j < 16; j++ ) kernel -> log( (uint8_t *) " %2X", ((uint8_t *) vfs)[ (16 * i) + j ] ); kernel -> log( (uint8_t *) "\n" ); }

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
	uint64_t file_id = task -> directory;

	// start from root directory?
	if( *path == STD_ASCII_SLASH ) return storage -> fs.root_directory_id;	// yes

	// file not found
	return file_id;
}

struct LIB_VFS_STRUCTURE kernel_qfs_search( struct KERNEL_STRUCTURE_STORAGE *storage, struct LIB_VFS_STRUCTURE *directory, uint8_t *name, uint64_t name_length ) {
	// located file properties
	struct LIB_VFS_STRUCTURE file = { EMPTY };

	// share file properties
	for( uint64_t b = 0; b < (directory -> limit >> STD_SHIFT_PAGE); b++ ) {
		// properties of directory entry
		struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) kernel_qfs_block( storage, directory, b );

		// for every possible entry
		for( uint8_t e = 0; e < STD_PAGE_byte / sizeof( struct LIB_VFS_STRUCTURE ); e++ ) {
			// file located?
			if( vfs[ e ].name_limit == name_length && lib_string_compare( (uint8_t *) vfs[ e ].name, name, name_length ) ) {
				// yes
				file = vfs[ e ];

				// ignore further search
				break;
			}
		}

		// release data block
		kernel -> memory_release( (uintptr_t) vfs, TRUE );

		// if file located
		if( file.name_limit ) break;	// ignore further search
	}

	// not located
	return file;
}

struct LIB_VFS_STRUCTURE kernel_qfs_touch( struct KERNEL_STRUCTURE_STORAGE *storage, uint8_t *path, uint64_t length, uint8_t type ) {
	// pointer to last file name inside path
	uint8_t *file_name = lib_string_basename( path );

	// file name length
	uint64_t file_name_length = length - ((uintptr_t) file_name - (uintptr_t) path);

	// open destination directory
	uint64_t directory_id = kernel_qfs_path( storage, path, length - file_name_length );
	struct LIB_VFS_STRUCTURE directory = kernel_qfs_file( storage, directory_id );

	// check if file already exist
	struct LIB_VFS_STRUCTURE file = kernel_qfs_search( storage, (struct LIB_VFS_STRUCTURE *) &directory, file_name, file_name_length );
	if( file.name_limit ) return file;	// exist

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
				kernel_qfs_create( storage, directory_id, (struct LIB_VFS_STRUCTURE *) &vfs[ e ], file_name, file_name_length, type );

				// update block content
				storage -> block_write( storage -> device_id, storage -> device_block + (kernel_qfs_block_id( storage, (struct LIB_VFS_STRUCTURE *) &directory, b ) * (LIB_VFS_BLOCK_byte / storage -> device_byte)), (uint8_t *) vfs, LIB_VFS_BLOCK_byte / storage -> device_byte );

				// return properties of file
				file = vfs[ e ];

				// file created, block updated
				break;
			}

		// extend search?
		// if( ! kernel_qfs_block( storage, (struct LIB_VFS_STRUCTURE *) &directory, b + 1 ) ) {
		// 	// expand directory content by block
		// 	kernel_qfs_block_fill( storage, (struct LIB_VFS_STRUCTURE *) &directory, ++blocks );

		// 	// new directory size
		// 	directory.limit += STD_PAGE_byte;
		// }

		// release data block
		kernel -> memory_release( (uintptr_t) vfs, TRUE );

		// if file created
		if( file.name_limit ) break;	// ignore further search
	}

	// no free entry
	return file;
}

void kernel_qfs_create( struct KERNEL_STRUCTURE_STORAGE *storage, uint64_t directory_id, struct LIB_VFS_STRUCTURE *vfs, uint8_t *name, uint64_t limit, uint8_t type ) {
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

			// current symlink
			dir[ 0 ].block[ FALSE ]	= EMPTY;	// pointing to superblock!
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
