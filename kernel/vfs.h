/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_VFS
	#define	KERNEL_VFS

	#define	KERNEL_VFS_limit		(STD_PAGE_byte / sizeof( struct KERNEL_STRUCTURE_VFS ))

	struct	KERNEL_STRUCTURE_VFS {
		uint64_t			storage;
		struct LIB_VFS_STRUCTURE	*knot;
		int64_t				pid;
		uint64_t			lock;		// amount of opened instances
		uint8_t				semaphore;	// set if someone is writing or reallocating file area
	};

	struct	KERNEL_STRUCTURE_VFS_PROPERTIES {
		uint64_t			byte;
		uint8_t				name_length;
		uint8_t				name[ LIB_VFS_NAME_limit + 1 ];
	};

	uintptr_t kernel_vfs_block_by_id( struct LIB_VFS_STRUCTURE *vfs, uint64_t i );

	void kernel_vfs_file_close( struct KERNEL_STRUCTURE_VFS *socket );

	struct KERNEL_STRUCTURE_VFS *kernel_vfs_file_open( uint8_t *path, uint64_t length );

	void kernel_vfs_file_properties( struct KERNEL_STRUCTURE_VFS *socket, struct KERNEL_STRUCTURE_VFS_PROPERTIES *properties );

	void kernel_vfs_file_read( struct KERNEL_STRUCTURE_VFS *socket, uint8_t *target, uint64_t seek, uint64_t length_byte );

	struct KERNEL_STRUCTURE_VFS *kernel_vfs_file_touch( uint8_t *path, uint8_t type );

	void kernel_vfs_file_write( struct KERNEL_STRUCTURE_VFS *socket, uint8_t *source, uint64_t seek, uint64_t byte );

	uint8_t	kernel_vfs_identify( uintptr_t base_address, uint64_t limit_byte );

	struct LIB_VFS_STRUCTURE *kernel_vfs_path( uint8_t *path, uint64_t length );

	struct KERNEL_STRUCTURE_VFS *kernel_vfs_socket_add( struct LIB_VFS_STRUCTURE *knot );
#endif