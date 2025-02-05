/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	MODULE_VIRTIO_NET
	#define	MODULE_VIRTIO_NET

	#define	MODULE_VIRTIO_NETWORK_FEATURE_MAC			(1 << 5)
	#define	MODULE_VIRTIO_NETWORK_FEATURE_MRG_RXBUF		(1 << 15)
	#define	MODULE_VIRTIO_NETWORK_FEATURE_STATUS		(1 << 16)

	#define	MODULE_VIRTIO_NETWORK_DESCRIPTOR_FLAG_READ		EMPTY
	#define	MODULE_VIRTIO_NETWORK_DESCRIPTOR_FLAG_NEXT		(1 << 0)
	#define	MODULE_VIRTIO_NETWORK_DESCRIPTOR_FLAG_WRITE		(1 << 1)
	#define	MODULE_VIRTIO_NETWORK_DESCRIPTOR_FLAG_INDIRECT	(1 << 3)

	#define	MODULE_VIRTIO_NETWORK_QUEUE_FLAG_interrupt_no	(1 << 0)	// don't inform us about device borrowing descriptor entry

	enum MODULE_VIRTIO_NETWORK_QUEUE {
		MODULE_VIRTIO_NETWORK_QUEUE_RX,
		MODULE_VIRTIO_NETWORK_QUEUE_TX
	};

	struct MODULE_VIRTIO_NETWORK_STRUTURE_NETWORK_QUEUE {
		struct MODULE_VIRTIO_STRUCTURE_DESCRIPTOR	*descriptor_address;
		uint16_t					descriptor_index;
		struct MODULE_VIRTIO_STRUCTURE_DRIVER		*driver_address;
		struct MODULE_VIRTIO_STRUCTURE_DEVICE		*device_address;
		uint16_t					device_index;
	};

	struct MODULE_VIRTIO_NETWORK_STRUCTURE_NETWORK {
		uint8_t						mac[ 6 ];
		struct MODULE_VIRTIO_NETWORK_STRUTURE_NETWORK_QUEUE	queue[ 2 ];
		uint16_t					queue_limit[ 2 ];
	};

	struct MODULE_VIRTIO_STRUCTURE_DEVICE_CONFIG {
		uint8_t		mac[ 6 ];
		uint16_t	status;
	} __attribute__( (packed) );

	struct MODULE_VIRTIO_NETWORK_STRUCTURE_HEADER {
		uint8_t		flags;
		uint8_t		gso_type;
		uint16_t	header_length;
		uint16_t	gso_size;
		uint16_t	csum_start;
		uint16_t	csum_limit;
		// MODULE_VIRTIO_NETWORK_FEATURE_MRG_RXBUF is ignored by Qemu
		// uint16_t	num_buffers;	// so, no support
	} __attribute__( (packed) );

	// external routines (assembly language)
	extern void module_virtio_network_entry( void );
#endif