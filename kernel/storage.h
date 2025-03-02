/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_STORAGE
	#define	KERNEL_STORAGE

	// ignore more storages than hard limit
	#define	KERNEL_STORAGE_limit			(STD_PAGE_byte / sizeof( struct KERNEL_STRUCTURE_STORAGE ))

	#define	KERNEL_STORAGE_FLAGS_active		(1 << 0)
	#define	KERNEL_STORAGE_FLAGS_reserved		(1 << 7)

	#define	KERNEL_STORAGE_NAME_limit		34

	// storage type:
	#define	KERNEL_STORAGE_FS_undefinied		EMPTY
	#define	KERNEL_STORAGE_FS_vfs			0x01	// Virtual File System
	#define	KERNEL_STORAGE_FS_qfs			0x02	// Quark File System
	#define	KERNEL_STORAGE_FS_raw			0xFF	// not set

	struct KERNEL_STRUCTURE_STORAGE_FS {
		struct KERNEL_STRUCTURE_VFS	*(*open)( uint64_t storage_id, uint8_t *path, uint64_t length, uint8_t mode );
		void	(*close)( void );
		void	(*file)( void );
		void	(*read)( void );
		void	(*write)( void );
		struct LIB_VFS_STRUCTURE *(*dir)( uint64_t storage_id, uint8_t *path, uint64_t length );
	};

	struct KERNEL_STRUCTURE_STORAGE {
		uint8_t					flags;
		uint8_t					device_type;
		uint8_t					device_fs;
		uint8_t					device_id;
		uint8_t					device_name_limit;
		uint8_t					device_name[ KERNEL_STORAGE_NAME_limit + 1 ];
		uint64_t				device_block;	// first usable block of storage
		uint64_t				device_limit;	// size of storage CLASS in blocks
		uint64_t				device_byte;	// size of single BLOCK in Bytes
		void					(*block_read)( uint64_t id, uint64_t block, uint8_t *target, uint64_t length );
		void					(*block_write)( uint64_t id, uint64_t block, uint8_t *source, uint64_t length );
		struct KERNEL_STRUCTURE_STORAGE_FS	fs;
	};
#endif
