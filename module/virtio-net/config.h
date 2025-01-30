/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	MODULE_VIRTIO_NET
	#define	MODULE_VIRTIO_NET

	#define	MODULE_VIRTIO_NET_FEATURE_MAC			(1 << 5)
	#define	MODULE_VIRTIO_NET_FEATURE_MRG_RXBUF		(1 << 15)
	#define	MODULE_VIRTIO_NET_FEATURE_STATUS		(1 << 16)

	#define	MODULE_VIRTIO_NET_DESCRIPTOR_FLAG_READ		EMPTY
	#define	MODULE_VIRTIO_NET_DESCRIPTOR_FLAG_NEXT		(1 << 0)
	#define	MODULE_VIRTIO_NET_DESCRIPTOR_FLAG_WRITE		(1 << 1)
	#define	MODULE_VIRTIO_NET_DESCRIPTOR_FLAG_INDIRECT	(1 << 3)

	#define	MODULE_VIRTIO_NET_QUEUE_FLAG_interrupt_no	(1 << 0)	// don't inform us about device borrowing the cache entry

	enum MODULE_VIRTIO_NET_QUEUE {
		MODULE_VIRTIO_NET_QUEUE_RX,
		MODULE_VIRTIO_NET_QUEUE_TX
	};

	struct MODULE_VIRTIO_NET_STRUTURE_NETWORK_QUEUE {
		struct MODULE_VIRTIO_NET_STRUCTURE_DESCRIPTOR	*descriptor_address;
		uint16_t					descriptor_index;
		struct MODULE_VIRTIO_NET_STRUCTURE_DRIVER		*available_address;
		struct MODULE_VIRTIO_NET_STRUCTURE_DEVICE		*used_address;
		uint16_t					used_index;
	};

	struct MODULE_VIRTIO_NET_STRUCTURE_NETWORK {
		uint16_t			subsystem_id;
		struct DRIVER_PCI_STRUCTURE	pci;

		uintptr_t			base_address;
		uintptr_t			mmio_address;
		uint8_t				limit;
		uint8_t				mmio_semaphore;
		uint8_t				irq;
		uint8_t				mac[ 6 ];
		struct MODULE_VIRTIO_NET_STRUTURE_NETWORK_QUEUE	queue[ 2 ];
		uint16_t			queue_limit[ 2 ];
	};

	struct MODULE_VIRTIO_NET_STRUCTURE_NETWORK_DEVICE_CONFIG {
		uint8_t		mac[ 6 ];
		uint16_t	status;
	} __attribute__( (packed) );

	struct MODULE_VIRTIO_NET_STRUCTURE_DESCRIPTOR {
		uint64_t	address;
		uint32_t	limit;
		uint16_t	flags;
		uint16_t	next;
	} __attribute__( (packed) );

	struct MODULE_VIRTIO_NET_STRUCTURE_DRIVER {
		uint16_t	flags;
		uint16_t	index;
		uint16_t	*ring;
	} __attribute__( (packed) );

	struct MODULE_VIRTIO_NET_STRUCTURE_RING {
		uint32_t	index;
		uint32_t	length;
	} __attribute__( (packed) );

	struct MODULE_VIRTIO_NET_STRUCTURE_DEVICE {
		uint16_t	flags;
		uint16_t	index;
		struct MODULE_VIRTIO_NET_STRUCTURE_RING *ring;
	} __attribute__( (packed) );

	struct MODULE_VIRTIO_NET_STRUCTURE_HEADER {
		uint8_t		flags;
		uint8_t		gso_type;
		uint16_t	header_length;
		uint16_t	gso_size;
		uint16_t	csum_start;
		uint16_t	csum_limit;
		// MODULE_VIRTIO_NET_FEATURE_MRG_RXBUF is ignored by Qemu
		// uint16_t	num_buffers;	// so, no support
	} __attribute__( (packed) );

	// external routines (assembly language)
	extern void module_virtio_net_entry( void );
#endif