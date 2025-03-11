/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_QFS
	#define	KERNEL_QFS

	void kernel_qfs_format( struct KERNEL_STRUCTURE_STORAGE *storage );
	uint8_t kernel_qfs_identify( struct KERNEL_STRUCTURE_STORAGE *storage );
	struct LIB_VFS_STRUCTURE kernel_qfs_file( struct KERNEL_STRUCTURE_STORAGE *storage, uint64_t file_id );
	struct KERNEL_STRUCTURE_VFS *kernel_qfs_open( struct KERNEL_STRUCTURE_STORAGE *storage, uint8_t *path, uint64_t length, uint8_t mode );
	uint64_t kernel_qfs_search( struct KERNEL_STRUCTURE_STORAGE *storage, struct LIB_VFS_STRUCTURE *directory, uint8_t *name, uint64_t name_length );
	uintptr_t kernel_qfs_dir( struct KERNEL_STRUCTURE_STORAGE *storage, uint8_t *path, uint64_t length );
	uint64_t kernel_qfs_path( struct KERNEL_STRUCTURE_STORAGE *storage, uint8_t *path, uint64_t length );
	void kernel_qfs_create( struct KERNEL_STRUCTURE_STORAGE *storage, uint64_t directory_id, uint64_t file_id, struct LIB_VFS_STRUCTURE *vfs, uint8_t *name, uint64_t limit, uint8_t type );
	uint64_t kernel_qfs_touch( struct KERNEL_STRUCTURE_STORAGE *storage, uint8_t *path, uint64_t length, uint8_t type );
	void kernel_qfs_close( struct KERNEL_STRUCTURE_VFS *socket );
	struct LIB_VFS_STRUCTURE kernel_qfs_properties_by_socket( struct KERNEL_STRUCTURE_VFS *socket );
#endif