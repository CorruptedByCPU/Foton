/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	MODULE_VIRTIO_BLOCK
	#define	MODULE_VIRTIO_BLOCK

	#define MODULE_VIRTIO_BLOCK_REQUEST_TYPE_in		EMPTY
	#define MODULE_VIRTIO_BLOCK_REQUEST_TYPE_out		(1 << 0)

	#define MODULE_VIRTIO_BLOCK_REQUEST_STATUS_ok		(1 << 0)
	#define MODULE_VIRTIO_BLOCK_REQUEST_STATUS_error	(1 << 1)
	#define MODULE_VIRTIO_BLOCK_REQUEST_STATUS_no_support	(1 << 2)

	struct MODULE_VIRTIO_BLOCK_STRUCTURE {
		uint8_t					id;
		uint8_t					semaphore;
		struct MODULE_VIRTIO_STRUTURE_QUEUE	queue;
		uint16_t				queue_limit;
	};

	struct MODULE_VIRTIO_BLOCK_STRUCTURE_DEVICE_CONFIG {
		uint64_t	capacity;
	} __attribute__( (packed) );

	struct MODULE_VIRTIO_BLOCK_STRUCTURE_REQUEST {
		uint32_t	type;
		uint32_t	reserved;
		uint64_t	block;
		uint8_t		data[ 512 ];
		uint8_t		status;
	} __attribute__( (packed) );

	void module_virtio_block_request_read( uint64_t id, uint64_t sector, uint8_t *data, uint64_t length );
	void module_virtio_block_request_write( uint64_t id, uint64_t sector, uint8_t *data, uint64_t length );
	void module_virtio_block_transfer( uint8_t type, uint64_t id, uint64_t sector, uint8_t *data, uint64_t length );
#endif