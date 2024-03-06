/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_VFS
	#define	KERNEL_VFS

	void kernel_vfs_old_file( struct LIB_VFS_STRUCTURE *vfs, struct STD_FILE_OLD_STRUCTURE *file );

	void kernel_vfs_old_read( struct LIB_VFS_STRUCTURE *vfs, uintptr_t target_address );

	int64_t kernel_vfs_old_write( struct LIB_VFS_STRUCTURE *vfs, uintptr_t source_address, uint64_t length_byte );
#endif