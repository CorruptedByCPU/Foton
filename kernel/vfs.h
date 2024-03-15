/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_VFS
	#define	KERNEL_VFS

	#define	DEPRECATED_KERNEL_VFS_align	16
	#define	DEPRECATED_KERNEL_VFS_base		64
	#define	DEPRECATED_KERNEL_VFS_length	4
	#define	DEPRECATED_KERNEL_VFS_magic	0x53465623	// "#VFS"
	#define	DEPRECATED_KERNEL_VFS_name_limit	255
	#define	DEPRECATED_KERNEL_VFS_shift	6
	#define	DEPRECATED_KERNEL_VFS_default	2

	#define	DEPRECATED_KERNEL_VFS_BLOCK_size		4096

	struct EXCHANGE_KERNEL_VFS_STRUCTURE {
		uintptr_t	offset;
		uint64_t	byte;
		uint8_t		type;
		uint8_t		name_length;
		uint8_t		name[ EXCHANGE_LIB_VFS_name_limit ];
		uint16_t	DEPRECATED_mode;
		uint16_t	DEPRECATED_uid;
		uint16_t	DEPRECATED_guid;
	};

	// returns TRUE if at specified address is VFS structure
	uint8_t DEPRECATED_kernel_vfs_check( uintptr_t address, uint64_t size_byte );

	void DEPRECATED_kernel_vfs_file( struct EXCHANGE_KERNEL_VFS_STRUCTURE *vfs, struct DEPRECATED_STD_FILE_STRUCTURE *file );

	void DEPRECATED_kernel_vfs_read( struct EXCHANGE_KERNEL_VFS_STRUCTURE *vfs, uintptr_t target_address );

	int64_t DEPRECATED_kernel_vfs_write( struct EXCHANGE_KERNEL_VFS_STRUCTURE *vfs, uintptr_t source_address, uint64_t length_byte );
#endif