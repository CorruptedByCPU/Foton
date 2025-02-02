/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel
	//----------------------------------------------------------------------
	#include	"../kernel/config.h"
	#include	"../kernel/idt.h"
	#include	"../kernel/io_apic.h"
	#include	"../kernel/lapic.h"
	#include	"../kernel/page.h"
	//----------------------------------------------------------------------
	// drivers
	//----------------------------------------------------------------------
	#include	"../kernel/driver/port.h"
	#include	"../kernel/driver/port.c"
	#include	"../kernel/driver/pci.h"
	#include	"../kernel/driver/pci.c"
	//----------------------------------------------------------------------
	// variables, structures, definitions of module
	//----------------------------------------------------------------------
	#include	"./virtio.h"
	#include	"./virtio-blk/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./virtio-blk/data.c"

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// initialize VirtIO Block properties
	module_virtio_blk = (struct MODULE_VIRTIO_BLK_STRUCTURE *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_VIRTIO_BLK_STRUCTURE ) * TRUE ) >> STD_SHIFT_PAGE );

	// check every bus;device;function of PCI controller
	for( uint16_t b = 0; b < 256; b++ )
		for( uint8_t d = 0; d < 32; d++ )
			for( uint8_t f = 0; f < 8; f++ ) {
				//----------------------------------------------

				// PCI properties
				module_virtio_blk -> pci.result		= EMPTY;
				module_virtio_blk -> pci.bus		= b;
				module_virtio_blk -> pci.device		= d;
				module_virtio_blk -> pci.function	= f;

				// retrieve device-vendor properties
				uint32_t device_vendor = driver_pci_read( module_virtio_blk -> pci, DRIVER_PCI_REGISTER_vendor_and_device );

				// VirtIO controller?
				if( (uint16_t) device_vendor != DRIVER_PCI_DEVICE_VENDOR_virtio ) continue;	// no
				
				// debug
				kernel -> log( (uint8_t *) "VirtIO controller found at PCI %2X:%2X:%2X.\n", module_virtio_blk -> pci.bus, module_virtio_blk -> pci.device, module_virtio_blk -> pci.function );

				// Supported VirtIO device?
				if( ((device_vendor >> STD_MOVE_WORD) < 0x1000) && ((device_vendor >> STD_MOVE_WORD) > 0x107F) ) {	// no
					// debug
					kernel -> log( (uint8_t *) "Unsupported VirtIO device. (Vendor 0x%4X, Device 0x%4X)\n", (uint16_t) device_vendor, device_vendor >> STD_MOVE_WORD );

					// next
					continue;
				}

				// Modern device?
				if( ((device_vendor >> STD_MOVE_WORD) >= 0x1040) && ((device_vendor >> STD_MOVE_WORD) <= 0x107F) || ((uint8_t) driver_pci_read( module_virtio_blk -> pci, DRIVER_PCI_REGISTER_class_and_subclass ) > 0) || ((driver_pci_read( module_virtio_blk -> pci, DRIVER_PCI_REGISTER_subsystem_and_vendor_id ) >> STD_MOVE_WORD) >= 0x40) ) module_virito_device_modern = TRUE;	// yes

				// Transitional device?
				if( (device_vendor >> STD_MOVE_WORD) > 0x0FFF && (device_vendor >> STD_MOVE_WORD) < 0x1040 && (uint8_t) driver_pci_read( module_virtio_blk -> pci, DRIVER_PCI_REGISTER_class_and_subclass ) == 0 ) module_virtio_device_transitional = TRUE;	// yes

				// debug
				if( module_virito_device_modern ) kernel -> log( (uint8_t *) "Modern device.\n" );
				else if( module_virtio_device_transitional ) kernel -> log( (uint8_t *) "Transitional device.\n" );
				else kernel -> log( (uint8_t *) "Legacy device.\n" );

				// VirtIO-Blk device?
				if( module_virito_device_modern && (device_vendor >> STD_MOVE_WORD) != 0x1042 ) continue;	// no
				else if( module_virtio_device_transitional && (driver_pci_read( module_virtio_blk -> pci, DRIVER_PCI_REGISTER_subsystem_and_vendor_id ) >> STD_MOVE_WORD) != MODULE_VIRTIO_DEVICE_ID_block ) continue;	// no
				else if( (device_vendor >> STD_MOVE_WORD) != MODULE_VIRTIO_DEVICE_block ) continue;	// no 

				// debug
				kernel -> log( (uint8_t *) "VirtIO-Blk.\n" );

				// // register Network Controller area
				// module_virtio_blk -> base_address = driver_pci_read( module_virtio_blk -> pci, DRIVER_PCI_REGISTER_bar0 ) & 0xFFF0;

				// // retrieve interrupt number of device
				// module_virtio_blk -> irq = (uint8_t) driver_pci_read( module_virtio_blk -> pci, DRIVER_PCI_REGISTER_irq );

				// // properties of device features and status
				// uint64_t device_features;
				// uint8_t device_status;

				// //----------------------------------------------

				// // reset device
				// device_status = EMPTY;
				// driver_port_out_byte( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

				// //----------------------------------------------

				// // device recognized
				// device_status |= MODULE_VIRTIO_DEVICE_STATUS_acknowledge;
				// driver_port_out_byte( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

				// // driver available
				// device_status |= MODULE_VIRTIO_DEVICE_STATUS_driver_available;
				// driver_port_out_byte( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

				// //----------------------------------------------

				// // retrieve device features
				// device_features = driver_port_in_dword( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_device_features );

				// // MAC field?
				// if( ! (device_features & MODULE_VIRTIO_NET_FEATURE_MAC) ) {
				// 	// set own MAC address
				// 	module_virtio_blk -> mac[ 0 ] = 0x00; driver_port_out_byte( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NET_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 0 ] ), module_virtio_blk -> mac[ 0 ] );
				// 	module_virtio_blk -> mac[ 1 ] = 0x22; driver_port_out_byte( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NET_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 1 ] ), module_virtio_blk -> mac[ 1 ] );
				// 	module_virtio_blk -> mac[ 2 ] = 0x44; driver_port_out_byte( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NET_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 2 ] ), module_virtio_blk -> mac[ 2 ] );
				// 	module_virtio_blk -> mac[ 3 ] = 0x66; driver_port_out_byte( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NET_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 3 ] ), module_virtio_blk -> mac[ 3 ] );
				// 	module_virtio_blk -> mac[ 4 ] = 0x88; driver_port_out_byte( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NET_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 4 ] ), module_virtio_blk -> mac[ 4 ] );
				// 	module_virtio_blk -> mac[ 5 ] = 0xAA; driver_port_out_byte( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NET_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 5 ] ), module_virtio_blk -> mac[ 5 ] );
				// }
				
				// // Status field?
				// if( ! (device_features & MODULE_VIRTIO_NET_FEATURE_STATUS) ) continue;	// no

				// //----------------------------------------------

				// // inform about supported/required features by driver
				// uint32_t quest_features = MODULE_VIRTIO_NET_FEATURE_MAC | MODULE_VIRTIO_NET_FEATURE_STATUS;	// do not touch MODULE_VIRTIO_NET_FEATURE_MRG_RXBUF, Qemu ignores it
				// driver_port_out_dword( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_guest_features, device_features );

				// // close negotiations
				// device_status |= MODULE_VIRTIO_DEVICE_STATUS_features_ok;
				// driver_port_out_byte( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

				// // retrieve current device status
				// device_status = driver_port_in_byte( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_device_status );

				// // requested features, accepted?
				// if( ! (device_status & MODULE_VIRTIO_DEVICE_STATUS_features_ok) ) continue;	// no

				// //----------------------------------------------

				// // network adapter have 2 queues
				// for( uint16_t i = 0; i < 2; i++ ) {
				// 	// select queue
				// 	driver_port_out_word( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_queue_select, i );

				// 	// check if queue exist
				// 	module_virtio_blk -> queue_limit[ i ] = driver_port_in_word( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_queue_limit );
				// 	if( ! module_virtio_blk -> queue_limit[ i ] ) continue;	// doesn't

				// 	// rings sizes
				// 	uint64_t limit_cache = MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_VIRTIO_NET_STRUCTURE_DESCRIPTOR ) * module_virtio_blk -> queue_limit[ i ] );
				// 	uint64_t limit_available = MACRO_PAGE_ALIGN_UP( (sizeof( uint16_t ) * module_virtio_blk -> queue_limit[ i ]) + (3 * sizeof( uint16_t )) );
				// 	uint64_t limit_used = MACRO_PAGE_ALIGN_UP( (sizeof( struct MODULE_VIRTIO_NET_STRUCTURE_RING ) * module_virtio_blk -> queue_limit[ i ]) + (3 * sizeof( uint16_t )) );

				// 	// acquire area for queue
				// 	module_virtio_blk -> queue[ i ].descriptor_address = (struct MODULE_VIRTIO_NET_STRUCTURE_DESCRIPTOR *) (kernel -> memory_alloc_low( (limit_cache + limit_available + limit_used) >> STD_SHIFT_PAGE ) | KERNEL_PAGE_mirror);
				// 	module_virtio_blk -> queue[ i ].available_address = (struct MODULE_VIRTIO_NET_STRUCTURE_DRIVER *) ((uintptr_t) module_virtio_blk -> queue[ i ].descriptor_address + limit_cache);
				// 	module_virtio_blk -> queue[ i ].used_address = (struct MODULE_VIRTIO_NET_STRUCTURE_DEVICE *) ((uintptr_t) module_virtio_blk -> queue[ i ].descriptor_address + limit_cache + limit_available);

				// 	// register queue
				// 	driver_port_out_dword( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_queue_address, ((uintptr_t) module_virtio_blk -> queue[ i ].descriptor_address & ~KERNEL_PAGE_mirror) >> STD_SHIFT_PAGE );
				// }

				// //----------------------------------------------

				// // retrieve MAC address
				// module_virtio_blk -> mac[ 0 ] = driver_port_in_byte( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NET_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 0 ] ) );
				// module_virtio_blk -> mac[ 1 ] = driver_port_in_byte( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NET_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 1 ] ) );
				// module_virtio_blk -> mac[ 2 ] = driver_port_in_byte( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NET_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 2 ] ) );
				// module_virtio_blk -> mac[ 3 ] = driver_port_in_byte( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NET_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 3 ] ) );
				// module_virtio_blk -> mac[ 4 ] = driver_port_in_byte( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NET_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 4 ] ) );
				// module_virtio_blk -> mac[ 5 ] = driver_port_in_byte( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NET_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 5 ] ) );

				// // update
				// kernel -> network_interface.ethernet_address[ 0 ] = module_virtio_blk -> mac[ 0 ];
				// kernel -> network_interface.ethernet_address[ 1 ] = module_virtio_blk -> mac[ 1 ];
				// kernel -> network_interface.ethernet_address[ 2 ] = module_virtio_blk -> mac[ 2 ];
				// kernel -> network_interface.ethernet_address[ 3 ] = module_virtio_blk -> mac[ 3 ];
				// kernel -> network_interface.ethernet_address[ 4 ] = module_virtio_blk -> mac[ 4 ];
				// kernel -> network_interface.ethernet_address[ 5 ] = module_virtio_blk -> mac[ 5 ];

				// //----------------------------------------------

				// // connect network controller interrupt handler
				// kernel -> idt_mount( KERNEL_IDT_IRQ_offset + module_virtio_blk -> irq, KERNEL_IDT_TYPE_irq, (uintptr_t) module_virtio_blk_entry );

				// // connect interrupt vector from IDT table to IOAPIC controller
				// kernel -> io_apic_connect( KERNEL_IDT_IRQ_offset + module_virtio_blk -> irq, KERNEL_IO_APIC_iowin + (module_virtio_blk -> irq * 0x02) );

				// //----------------------------------------------

				// // fill Receive Queue
				// for( uint64_t i = 0; i < module_virtio_blk -> queue_limit[ MODULE_VIRTIO_NET_QUEUE_RX ]; i++ ) {
				// 	// allocate area in Descriptors Queue
				// 	module_virtio_blk -> queue[ MODULE_VIRTIO_NET_QUEUE_RX ].descriptor_address[ i ].address = (uintptr_t) kernel -> memory_alloc_page();
				// 	module_virtio_blk -> queue[ MODULE_VIRTIO_NET_QUEUE_RX ].descriptor_address[ i ].limit = STD_PAGE_byte;
				// 	module_virtio_blk -> queue[ MODULE_VIRTIO_NET_QUEUE_RX ].descriptor_address[ i ].flags = MODULE_VIRTIO_NET_DESCRIPTOR_FLAG_WRITE;

				// 	// add cache to available ring
				// 	uint16_t *receive_ring_available = (uint16_t *) (module_virtio_blk -> queue[ MODULE_VIRTIO_NET_QUEUE_RX ].available_address + offsetof( struct MODULE_VIRTIO_NET_STRUCTURE_DRIVER, ring ));
				// 	receive_ring_available[ i ] = i;

				// 	// synchronize memory with host
				// 	MACRO_SYNC();

				// 	// set next available index
				// 	module_virtio_blk -> queue[ MODULE_VIRTIO_NET_QUEUE_RX ].available_address -> index++;

				// 	// synchronize memory with host
				// 	MACRO_SYNC();
				// }

				// // inform about updated Receive Queue
				// driver_port_out_word( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_queue_notify, MODULE_VIRTIO_NET_QUEUE_RX );

				// // fill Transmit Queue
				// for( uint64_t i = 0; i < module_virtio_blk -> queue_limit[ MODULE_VIRTIO_NET_QUEUE_TX ]; i++ ) {
				// 	// allocate area in Descriptors Queue
				// 	module_virtio_blk -> queue[ MODULE_VIRTIO_NET_QUEUE_TX ].descriptor_address[ i ].address = (uintptr_t) kernel -> memory_alloc_page();
				// 	module_virtio_blk -> queue[ MODULE_VIRTIO_NET_QUEUE_TX ].descriptor_address[ i ].limit = STD_PAGE_byte;
				// 	module_virtio_blk -> queue[ MODULE_VIRTIO_NET_QUEUE_TX ].descriptor_address[ i ].flags = MODULE_VIRTIO_NET_DESCRIPTOR_FLAG_READ;

				// 	// synchronize memory with host
				// 	MACRO_SYNC();
				// }

				// // inform about updated Transmit Queue
				// driver_port_out_word( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_queue_notify, MODULE_VIRTIO_NET_QUEUE_TX );

				// //----------------------------------------------

				// // driver configured
				// device_status |= MODULE_VIRTIO_DEVICE_STATUS_driver_ok;
				// driver_port_out_byte( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

				// //----------------------------------------------

				// // transmit function ready?
				// while( ! kernel -> network_tx );

				// // hold the door
				// while( TRUE ) {
				// 	// properties of frame to send
				// 	uintptr_t frame = EMPTY;

				// 	// acquire data for transmission
				// 	while( ! (frame = kernel -> network_tx()) ) kernel -> time_sleep( TRUE );	// release AP time

				// 	// debug
				// 	// kernel -> log( (uint8_t *) "Tx\n" );

				// 	// resolve properties
				// 	uint8_t *data = (uint8_t *) (frame & STD_PAGE_mask | KERNEL_PAGE_mirror);
				// 	uint64_t length = frame & ~STD_PAGE_mask;

				// 	// move packet content behind header
				// 	if( length + sizeof( struct MODULE_VIRTIO_NET_STRUCTURE_HEADER ) >= STD_PAGE_byte ) {
				// 		// debug
				// 		kernel -> log( (uint8_t *) "[VIRTIO] Tx overflow.\n" );

				// 		// ignore packet
				// 		continue;
				// 	}

				// 	// properties of
				// 	uint16_t index = module_virtio_blk -> queue[ MODULE_VIRTIO_NET_QUEUE_TX ].available_address -> index % module_virtio_blk -> queue_limit[ MODULE_VIRTIO_NET_QUEUE_TX ];
				// 	struct MODULE_VIRTIO_NET_STRUCTURE_DESCRIPTOR *descriptor	= (struct MODULE_VIRTIO_NET_STRUCTURE_DESCRIPTOR *) &module_virtio_blk -> queue[ MODULE_VIRTIO_NET_QUEUE_TX ].descriptor_address[ index ];
				// 	struct MODULE_VIRTIO_NET_STRUCTURE_DRIVER *available	= (struct MODULE_VIRTIO_NET_STRUCTURE_DRIVER *) module_virtio_blk -> queue[ MODULE_VIRTIO_NET_QUEUE_TX ].available_address;
				// 	uint16_t *available_ring				= (uint16_t *) ((uintptr_t) available + offsetof( struct MODULE_VIRTIO_NET_STRUCTURE_DRIVER, ring ));

				// 	// copy packet content to descriptor
				// 	uint8_t *source = (uint8_t *) ((uintptr_t) data | KERNEL_PAGE_mirror);
				// 	uint8_t *target = (uint8_t *) (descriptor -> address | KERNEL_PAGE_mirror);
				// 	for( int64_t i = 0; i < length; i++ ) target[ i + sizeof( struct MODULE_VIRTIO_NET_STRUCTURE_HEADER ) ] = source[ i ];

				// 	// set descriptor
				// 	descriptor -> limit = length + sizeof( struct MODULE_VIRTIO_NET_STRUCTURE_HEADER );

				// 	// prepare header
				// 	struct MODULE_VIRTIO_NET_STRUCTURE_HEADER *header = (struct MODULE_VIRTIO_NET_STRUCTURE_HEADER *) target;
				// 	header -> flags		= EMPTY;
				// 	header -> gso_type	= EMPTY;
				// 	header -> header_length	= sizeof( struct MODULE_VIRTIO_NET_STRUCTURE_HEADER );
				// 	header -> gso_size	= EMPTY;
				// 	header -> csum_start	= EMPTY;
				// 	header -> csum_limit	= EMPTY;

				// 	// debug
				// 	// kernel -> log( (uint8_t *) "\n" ); for( uint64_t i = 0; i < (descriptor -> limit % 0x10) + TRUE; i++ ) { kernel -> log( (uint8_t *) "0x%8X", (uintptr_t) target + (i * 0x10) ); for( uint64_t k = i * 0x10; k < ((i * 0x10) + 0x10); k++ ) kernel -> log( (uint8_t *) " %2X", target[ k ] ); kernel -> log( (uint8_t *) "\n" ); }

				// 	// add to available ring
				// 	available_ring[ index ] = index;

				// 	// set next available index
				// 	available -> index++;

				// 	// synchronize memory with host
				// 	MACRO_SYNC();

				// 	// inform about updated available queue
				// 	driver_port_out_word( module_virtio_blk -> base_address + MODULE_VIRTIO_REGISTER_queue_notify, MODULE_VIRTIO_NET_QUEUE_TX );

				// 	// release frame
				// 	kernel -> memory_release_page( (uintptr_t) data );
				// }
			}

	// hodor
	while( TRUE );
}
