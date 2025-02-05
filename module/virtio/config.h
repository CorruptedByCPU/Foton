/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	MODULE_VIRTIO
	#define	MODULE_VIRTIO

	#define	MODULE_VIRTIO_TYPE_network		1
	#define	MODULE_VIRTIO_TYPE_block		2

	#define	MODULE_VIRTIO_DEVICE_network		0x1000
	#define	MODULE_VIRTIO_DEVICE_block		0x1001
	#define	MODULE_VIRTIO_DEVICE_ID_network		0x0001
	#define	MODULE_VIRTIO_DEVICE_ID_block		0x0002

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

	struct MODULE_VIRTIO_STRUCTURE {
		struct DRIVER_PCI_STRUCTURE	pci;
		uint8_t				type;
		uint8_t				semaphore_legacy;
		uint8_t				semaphore_transitional;
		uint8_t				semaphore_modern;
		uintptr_t			base_address;
		uint8_t				irq;
		uintptr_t			device;
	};

	struct MODULE_VIRTIO_STRUCTURE_DESCRIPTOR {
		uint64_t	address;
		uint32_t	limit;
		uint16_t	flags;
		uint16_t	next;
	} __attribute__( (packed) );

	struct MODULE_VIRTIO_STRUCTURE_DRIVER {
		uint16_t	flags;
		uint16_t	index;
		uint16_t	*ring;
	} __attribute__( (packed) );

	struct MODULE_VIRTIO_STRUCTURE_RING {
		uint32_t	index;
		uint32_t	length;
	} __attribute__( (packed) );

	struct MODULE_VIRTIO_STRUCTURE_DEVICE {
		uint16_t	flags;
		uint16_t	index;
		struct MODULE_VIRTIO_STRUCTURE_RING *ring;
	} __attribute__( (packed) );
#endif