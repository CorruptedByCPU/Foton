/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_VFS
	#define	KERNEL_VFS

	#define	KERNEL_VFS_align	16
	#define	KERNEL_VFS_base		64
	#define	KERNEL_VFS_length	4
	#define	KERNEL_VFS_magic	0x53465623	// "#VFS"
	#define	KERNEL_VFS_name_limit	255
	#define	KERNEL_VFS_shift	6
	#define	KERNEL_VFS_default	2

	#define	KERNEL_VFS_BLOCK_size		4096

	struct KERNEL_VFS_STRUCTURE {
		uintptr_t	offset;
		uint64_t	size;
		uint16_t	length;
		uint16_t	mode;
		uint16_t	uid;
		uint16_t	guid;
		uint8_t		type;
		uint8_t		name[ KERNEL_VFS_name_limit ];
	};

	// returns TRUE if at specified address is VFS structure
	uint8_t kernel_vfs_check( uintptr_t address, uint64_t size_byte );

	void kernel_vfs_file( struct KERNEL_VFS_STRUCTURE *vfs, struct STD_FILE_STRUCTURE *file );

	void kernel_vfs_read( struct KERNEL_VFS_STRUCTURE *vfs, uintptr_t target_address );

	int64_t kernel_vfs_write( struct KERNEL_VFS_STRUCTURE *vfs, uintptr_t source_address, uint64_t length_byte );
#endif