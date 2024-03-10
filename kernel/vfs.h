/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_VFS
	#define	KERNEL_VFS

	#define	KERNEL_VFS_limit	(STD_PAGE_byte / sizeof( struct KERNEL_VFS_STRUCTURE ))	// hard limit

	#define	KERNEL_VFS_MODE_read		0b00000001
	#define	KERNEL_VFS_MODE_write		0b00000010
	#define	KERNEL_VFS_MODE_reserved	0b10000000

	struct KERNEL_VFS_STRUCTURE {
		uint64_t	storage;	// id of storage where file/directory is stored
		int64_t		pid;		// to which process this socket belongs to
		uint8_t		mode;		// type of operation on file
		uint64_t	id;		// file identificator (we should be able to locate file on storage by this value)
		uint64_t	byte;		// size of file/directory(structure) in Bytes
	};

	struct KERNEL_VFS_STRUCTURE *kernel_vfs_file_open( uint8_t *path, uint64_t length, uint8_t mode );
	void kernel_vfs_file_close( struct KERNEL_VFS_STRUCTURE *socket );
	uint64_t kernel_vfs_socket( void );

// OLD ========================================================================

	void kernel_vfs_old_file( struct LIB_VFS_STRUCTURE *vfs, struct STD_FILE_OLD_STRUCTURE *file );

	void kernel_vfs_old_read( struct LIB_VFS_STRUCTURE *vfs, uintptr_t target_address );

	int64_t kernel_vfs_old_write( struct LIB_VFS_STRUCTURE *vfs, uintptr_t source_address, uint64_t length_byte );
#endif