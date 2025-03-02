/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_QFS
	#define	KERNEL_QFS

	void kernel_qfs_format( uint64_t storage_id );
	uint8_t kernel_qfs_identify( uint64_t storage_id );
	struct LIB_VFS_STRUCTURE kernel_qfs_file( uint64_t storage_id, uint64_t file_id );
	uint64_t kernel_qfs_open( uint64_t storage_id, uint8_t *path, uint64_t length, uint8_t mode );
	uint64_t kernel_qfs_search( uint64_t storage_id, uint64_t directory_id, uint8_t *name, uint64_t name_length );
#endif