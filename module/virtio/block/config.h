/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	MODULE_VIRTIO_BLK
	#define	MODULE_VIRTIO_BLK

	#define	MODULE_VIRTIO_BLK_DESCRIPTOR_FLAG_READ		EMPTY
	#define	MODULE_VIRTIO_BLK_DESCRIPTOR_FLAG_NEXT		(1 << 0)
	#define	MODULE_VIRTIO_BLK_DESCRIPTOR_FLAG_WRITE		(1 << 1)
	#define	MODULE_VIRTIO_BLK_DESCRIPTOR_FLAG_INDIRECT	(1 << 3)

	struct MODULE_VIRTIO_BLK_STRUTURE_QUEUE {
		struct MODULE_VIRTIO_BLK_STRUCTURE_DESCRIPTOR	*descriptor_address;
		uint16_t					descriptor_index;
		struct MODULE_VIRTIO_BLK_STRUCTURE_DRIVER	*driver_address;
		struct MODULE_VIRTIO_BLK_STRUCTURE_DEVICE	*device_address;
		uint16_t					device_index;
	};
	
	struct MODULE_VIRTIO_BLK_STRUCTURE {
		uint16_t			subsystem_id;
		struct DRIVER_PCI_STRUCTURE	pci;

		uintptr_t			base_address;
		uint8_t				limit;
		uint8_t				irq;
		struct MODULE_VIRTIO_BLK_STRUTURE_QUEUE	queue[ 2 ];
		uint16_t			queue_limit[ 2 ];
	};

	struct MODULE_VIRTIO_BLK_STRUCTURE_DESCRIPTOR {
		uint64_t	address;
		uint32_t	limit;
		uint16_t	flags;
		uint16_t	next;
	} __attribute__( (packed) );

	struct MODULE_VIRTIO_BLK_STRUCTURE_DRIVER {
		uint16_t	flags;
		uint16_t	index;
		uint16_t	*ring;
	} __attribute__( (packed) );

	struct MODULE_VIRTIO_BLK_STRUCTURE_RING {
		uint32_t	index;
		uint32_t	length;
	} __attribute__( (packed) );

	struct MODULE_VIRTIO_BLK_STRUCTURE_DEVICE {
		uint16_t	flags;
		uint16_t	index;
		struct MODULE_VIRTIO_BLK_STRUCTURE_RING *ring;
	} __attribute__( (packed) );
#endif