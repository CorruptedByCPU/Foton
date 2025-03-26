/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_VFS
	#define	KERNEL_VFS

	#define	KERNEL_VFS_limit		(STD_PAGE_byte / sizeof( struct KERNEL_STRUCTURE_VFS ))

	struct	KERNEL_STRUCTURE_VFS {
		uint64_t			storage;
		uint64_t			knot;
		int64_t				pid;
		uint8_t				mode;
	};

	uintptr_t kernel_vfs_block_by_id( struct LIB_VFS_STRUCTURE *vfs, uint64_t i );

	void kernel_vfs_file_close( struct KERNEL_STRUCTURE_VFS *socket );

	struct KERNEL_STRUCTURE_VFS *kernel_vfs_file_open( struct KERNEL_STRUCTURE_STORAGE *storage, uint8_t *path, uint64_t length, uint8_t mode );

	struct LIB_VFS_STRUCTURE kernel_vfs_file_properties( struct KERNEL_STRUCTURE_VFS *socket );

	void kernel_vfs_file_read( struct KERNEL_STRUCTURE_VFS *socket, uint8_t *target, uint64_t seek, uint64_t length_byte );

	struct KERNEL_STRUCTURE_VFS *kernel_vfs_touch( struct KERNEL_STRUCTURE_STORAGE *storage, uint8_t *path, uint64_t length, uint8_t type );

	void kernel_vfs_file_write( struct KERNEL_STRUCTURE_VFS *socket, uint8_t *source, uint64_t seek, uint64_t byte );

	uint8_t	kernel_vfs_identify( uintptr_t base_address, uint64_t limit_byte );

	struct LIB_VFS_STRUCTURE *kernel_vfs_path( uint64_t storage_id, uint8_t *path, uint64_t length );

	struct KERNEL_STRUCTURE_VFS *kernel_vfs_socket_add( void );

	uintptr_t kernel_vfs_dir( uint64_t storage_id, uint8_t *path, uint64_t length );


	void kernel_vfs_read( uint64_t reserved, uint64_t block, uint8_t *target, uint64_t length );
	void kernel_vfs_write( uint64_t reserved, uint64_t block, uint8_t *source, uint64_t length );
#endif
