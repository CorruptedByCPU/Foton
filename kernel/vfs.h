/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_VFS
	#define	KERNEL_VFS

	#define	KERNEL_VFS_limit	(STD_PAGE_byte / sizeof( struct KERNEL_VFS_STRUCTURE ))	// hard limit

	#define	KERNEL_VFS_FLAG_reserved	0b10000000

	struct KERNEL_VFS_STRUCTURE {
		uint8_t	flags;
		int64_t	pid;
	};

	struct KERNEL_VFS_STRUCTURE *kernel_vfs_file_open( uint8_t *path, uint64_t length );
	void kernel_vfs_file_close( struct KERNEL_VFS_STRUCTURE *socket );

// OLD ========================================================================

	void kernel_vfs_old_file( struct LIB_VFS_STRUCTURE *vfs, struct STD_FILE_OLD_STRUCTURE *file );

	void kernel_vfs_old_read( struct LIB_VFS_STRUCTURE *vfs, uintptr_t target_address );

	int64_t kernel_vfs_old_write( struct LIB_VFS_STRUCTURE *vfs, uintptr_t source_address, uint64_t length_byte );
#endif