/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_VFS
	#define	KERNEL_VFS

	#define	KERNEL_VFS_limit		(STD_PAGE_byte / sizeof( struct KERNEL_VFS_STRUCTURE ))

	#define	KERNEL_VFS_MODE_reserved	1
	#define	KERNEL_VFS_MODE_read	2
	
	struct	KERNEL_VFS_STRUCTURE {
		uint64_t	storage;
		uint64_t	knot;
		int64_t		pid;
		uint8_t		mode;
	};

	struct	KERNEL_VFS_STRUCTURE_PROPERTIES {
		uint64_t	byte;
		uint8_t		name_length;
		uint8_t		name[ LIB_VFS_NAME_limit ];
	};

	void kernel_vfs_file_close( struct KERNEL_VFS_STRUCTURE *socket );

	struct KERNEL_VFS_STRUCTURE *kernel_vfs_file_open( uint8_t *path, uint64_t length, uint8_t mode );

	void kernel_vfs_file_properties( struct KERNEL_VFS_STRUCTURE *socket, struct KERNEL_VFS_STRUCTURE_PROPERTIES *properties );

	void kernel_vfs_file_read( struct KERNEL_VFS_STRUCTURE *socket, uint8_t *target, uint64_t seek, uint64_t length_byte );

	uint8_t	kernel_vfs_identify( uintptr_t base_address, uint64_t limit_byte );

	uint64_t kernel_vfs_socket_add( void );
#endif