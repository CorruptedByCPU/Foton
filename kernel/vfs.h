/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_VFS
	#define	KERNEL_VFS

	#define	NEW_KERNEL_VFS_limit		(STD_PAGE_byte / sizeof( struct EXCHANGE_LIB_VFS_STRUCTURE ))

	#define	NEW_KERNEL_VFS_MODE_reserved	1
	#define	NEW_KERNEL_VFS_MODE_read	2
	
	struct	NEW_KERNEL_VFS_STRUCTURE {
		uint64_t	storage;
		uint64_t	knot;
		uint64_t	byte;
		int64_t		pid;
		uint8_t		mode;
	};

	struct	NEW_KERNEL_VFS_STRUCTURE_PROPERTIES {
		uint64_t	byte;
		uint8_t		name_length;
		uint8_t		name[ EXCHANGE_LIB_VFS_NAME_limit ];
	};

	void NEW_kernel_vfs_file_close( struct NEW_KERNEL_VFS_STRUCTURE *socket );

	struct NEW_KERNEL_VFS_STRUCTURE *NEW_kernel_vfs_file_open( uint8_t *path, uint64_t length, uint8_t mode );

	void NEW_kernel_vfs_file_properties( struct NEW_KERNEL_VFS_STRUCTURE *socket, struct NEW_KERNEL_VFS_STRUCTURE_PROPERTIES *properties );

	void NEW_kernel_vfs_file_read( struct NEW_KERNEL_VFS_STRUCTURE *socket, uint8_t *target, uint64_t seek, uint64_t length_byte );

	uint8_t	NEW_kernel_vfs_identify( uintptr_t base_address, uint64_t limit_byte );

	uint64_t NEW_kernel_vfs_socket_add( void );

// OLD ========================================================================

	#define	DEPRECATED_KERNEL_VFS_align	16
	#define	DEPRECATED_KERNEL_VFS_base		64
	#define	DEPRECATED_KERNEL_VFS_length	4
	#define	DEPRECATED_KERNEL_VFS_magic	0x53465623	// "#VFS"
	#define	DEPRECATED_KERNEL_VFS_shift	6
	#define	DEPRECATED_KERNEL_VFS_default	2

	#define	DEPRECATED_KERNEL_VFS_BLOCK_size		4096

	// returns TRUE if at specified address is VFS structure
	uint8_t DEPRECATED_kernel_vfs_check( uintptr_t address, uint64_t size_byte );

	void DEPRECATED_kernel_vfs_file( struct EXCHANGE_LIB_VFS_STRUCTURE *vfs, struct DEPRECATED_STD_FILE_STRUCTURE *file );

	void DEPRECATED_kernel_vfs_read( struct EXCHANGE_LIB_VFS_STRUCTURE *vfs, uintptr_t target_address );

	int64_t DEPRECATED_kernel_vfs_write( struct EXCHANGE_LIB_VFS_STRUCTURE *vfs, uintptr_t source_address, uint64_t length_byte );
#endif