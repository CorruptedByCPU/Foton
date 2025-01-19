/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	MODULE_VIRTIO
	#define	MODULE_VIRTIO

	#define	MODULE_VIRTIO_SUBSYSTEM_ID_NETWORK	0x01
	#define	MODULE_VIRTIO_SUBSYSTEM_ID_BLOCK	0x02
	#define	MODULE_VIRTIO_SUBSYSTEM_ID_CONSOLE	0x03
	#define	MODULE_VIRTIO_SUBSYSTEM_ID_ENTROPY	0x04
	#define	MODULE_VIRTIO_SUBSYSTEM_ID_MEMORY	0x05
	#define	MODULE_VIRTIO_SUBSYSTEM_ID_MEMORY_IO	0x06
	#define	MODULE_VIRTIO_SUBSYSTEM_ID_RPMSG	0x07
	#define	MODULE_VIRTIO_SUBSYSTEM_ID_SCSI		0x08
	#define	MODULE_VIRTIO_SUBSYSTEM_ID_TRANSPORT_9P	0x09
	#define	MODULE_VIRTIO_SUBSYSTEM_ID_WLAN		0x0A

	#define	MODULE_VIRTIO_REGISTER_device_features	0x00
	#define	MODULE_VIRTIO_REGISTER_guest_features	0x04
	#define	MODULE_VIRTIO_REGISTER_queue_address	0x08
	#define	MODULE_VIRTIO_REGISTER_queue_limit	0x0C
	#define	MODULE_VIRTIO_REGISTER_queue_select	0x0E
	#define	MODULE_VIRTIO_REGISTER_queue_notify	0x10
	#define	MODULE_VIRTIO_REGISTER_device_status	0x12
	#define	MODULE_VIRTIO_REGISTER_isr_status	0x13
	#define	MODULE_VIRTIO_REGISTER_device_config	0x14

	#define	MODULE_VIRTIO_DEVICE_STATUS_acknowledge		(1 << 0)
	#define	MODULE_VIRTIO_DEVICE_STATUS_driver_available	(1 << 1)
	#define	MODULE_VIRTIO_DEVICE_STATUS_driver_ok		(1 << 2)
	#define	MODULE_VIRTIO_DEVICE_STATUS_features_ok		(1 << 3)
	#define	MODULE_VIRTIO_DEVICE_STATUS_device_needs_reset	(1 << 6)
	#define	MODULE_VIRTIO_DEVICE_STATUS_failed		(1 << 7)

	#define	MODULE_VIRTIO_DEVICE_FEATURE_mac		(1 << 5)
	#define	MODULE_VIRTIO_DEVICE_FEATURE_status		(1 << 16)

	#define	MODULE_VIRTIO_NET_CACHE_FLAG_next		(1 << 0)
	#define	MODULE_VIRTIO_NET_CACHE_FLAG_write		(1 << 1)
	#define	MODULE_VIRTIO_NET_CACHE_FLAG_indirect		(1 << 3)

	#define	MODULE_VIRTIO_NET_AVAILABLE_FLAG_interrupt_no	(1 << 0)	// don't inform us about device borrowing the cache entry

	struct MODULE_VIRTIO_STRUTURE_NETWORK_QUEUE {
		struct MODULE_VIRTIO_STRUCTURE_CACHE		*cache_address;
		struct MODULE_VIRTIO_STRUCTURE_AVAILABLE	*available_address;
		struct MODULE_VIRTIO_STRUCTURE_USED		*used_address;
	};

	struct MODULE_VIRTIO_STRUCTURE_NETWORK {
		uint16_t			subsystem_id;
		struct DRIVER_PCI_STRUCTURE	pci;

		uintptr_t			base_address;
		uint8_t				limit;
		uint8_t				mmio_semaphore;
		uint8_t				irq;
		uint8_t				mac[ 6 ];
		struct MODULE_VIRTIO_STRUTURE_NETWORK_QUEUE	queue[ 2 ];
		uint16_t			queue_limit[ 2 ];
	};

	struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG {
		uint8_t		mac[ 6 ];
		uint16_t	status;
	} __attribute__( (packed) );

	struct MODULE_VIRTIO_STRUCTURE_CACHE {
		uint64_t	address;
		uint32_t	limit;
		uint16_t	flags;
		uint16_t	next;
	} __attribute__( (packed) );

	struct MODULE_VIRTIO_STRUCTURE_AVAILABLE {
		uint16_t	flags;
		uint16_t	index;
		uint16_t	*ring;
		uint16_t	used_index;
	} __attribute__( (packed) );

	struct MODULE_VIRTIO_STRUCTURE_RING {
		uint32_t	index;
		uint32_t	length;
	} __attribute__( (packed) );

	struct MODULE_VIRTIO_STRUCTURE_USED {
		uint16_t	flags;
		uint16_t	index;
		struct MODULE_VIRTIO_STRUCTURE_RING *ring;
		uint16_t	available_event;
	} __attribute__( (packed) );

	struct MODULE_VIRTIO_NET_STRUCTURE_HEADER {
		uint8_t		flags;
		uint8_t		gso_type;
		uint16_t	header_length;
		uint16_t	gso_size;
		uint16_t	checksum_start;
		uint16_t	checksum_offset;
	} __attribute__( (packed) );

	// external routines (assembly language)
	extern void module_virtio_net_entry( void );
#endif