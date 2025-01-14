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

	struct MODULE_VIRTIO_STRUCTURE_NETWORK {
		uint16_t			subsystem_id;
		struct DRIVER_PCI_STRUCTURE	pci;

		uintptr_t			base_address;
		uint8_t				limit;
		uint8_t				mmio_semaphore;
	};

	struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG {
		uint8_t		mac[ 6 ];
		uint16_t	status;
		uint16_t	max_virtqueue_pairs;
		uint16_t	mtu;
	} __attribute__( (packed) );
#endif