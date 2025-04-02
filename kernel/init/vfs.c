/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t	kernel_init_vfs_identify( uintptr_t base, uint64_t limit ) {
	// file properties in chunks
	uint32_t *vfs = (uint32_t *) base;

	// at end of file, magic value exist?
	if( vfs[ (limit >> STD_SHIFT_4) - 1 ] == LIB_VFS_magic ) return TRUE;	// yes

	// no
	return FALSE;
}

uint64_t kernel_init_vfs_length( uintptr_t base_address ) {
	// detected size of directory
	uint64_t limit = INIT;

	// set pointer to directory content
	struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) base_address;

	// for every entry
	do {
		// expand limit by VFS entry
		limit += sizeof( struct LIB_VFS_STRUCTURE );
	// until empty one
	} while( (vfs++) -> type );

	// directory length
	return MACRO_PAGE_ALIGN_UP( limit );
}

void kernel_init_vfs_realloc( struct LIB_VFS_STRUCTURE *vfs, uintptr_t offset ) {
	// directory blocks
	uint64_t b = vfs -> limit >> STD_SHIFT_PAGE;

	// true block offset
	offset += vfs -> block[ FALSE ];
	vfs -> block[ 0 ] = offset;

	// no more blocks?
	if( ! --b ) return;	// yes

	// next block address
	offset += STD_PAGE_byte;

	// craete indirect block
	vfs -> block[ 1 ] = kernel_memory_alloc( TRUE );

	// indirect block properties
	uintptr_t *indirect = (uintptr_t *) vfs -> block[ 1 ];

	// parse indirect blocks
	for( uint16_t i = INIT; i < 512; i++ ) {
		// true block offset
		indirect[ i ] = offset;

		// next block address
		offset += STD_PAGE_byte;

		// no more blocks?
		if( ! --b ) return;	// yes
	}

	// no support for double-indirect and so on, yet
	// file of 2 MiB is enough for now
}

void kernel_init_vfs( void ) {
	// allocate area for list of open files
	kernel -> vfs_limit = KERNEL_VFS_limit;
	kernel -> vfs_base_address = (struct KERNEL_STRUCTURE_VFS *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( kernel -> vfs_limit * sizeof( struct KERNEL_STRUCTURE_VFS ) ) >> STD_SHIFT_PAGE );

	// detect modules with Virtual File System (like initramfs from GNU/Linux)
	for( uint64_t i = INIT; i < limine_module_request.response -> module_count; i++ ) {
		// VFS module type?
		if( ! kernel_init_vfs_identify( (uintptr_t) limine_module_request.response -> modules[ i ] -> address, (uint64_t) limine_module_request.response -> modules[ i ] -> size ) ) continue;	// no

		// create root superblock for VFS
		struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( LIB_VFS_BLOCK_byte ) >> STD_SHIFT_PAGE );
	
		// default properties of superblock
		vfs -> type		= STD_FILE_TYPE_directory;
		vfs -> name_limit	= TRUE; *vfs -> name = STD_ASCII_SLASH;
		vfs -> limit		= kernel_init_vfs_length( (uintptr_t) limine_module_request.response -> modules[ i ] -> address );

		// realloc blocks of VFS
		kernel_init_vfs_realloc( vfs, (uintptr_t) limine_module_request.response -> modules[ i ] -> address );
	}
}
