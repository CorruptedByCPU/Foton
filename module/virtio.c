/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel
	//----------------------------------------------------------------------
	#include	"../kernel/config.h"
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
	#include	"./virtio/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./virtio/data.c"

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// initialize VirtIO Network properties
	module_virtio_network = (struct MODULE_VIRTIO_STRUCTURE_NETWORK *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_VIRTIO_STRUCTURE_NETWORK ) * TRUE ) >> STD_SHIFT_PAGE );

	// check every bus;device;function of PCI controller
	for( uint16_t b = 0; b < 256; b++ )
		for( uint8_t d = 0; d < 32; d++ )
			for( uint8_t f = 0; f < 8; f++ ) {
				//----------------------------------------------

				// PCI properties
				module_virtio_network[ module_virtio_network_limit ].pci.result		= EMPTY;
				module_virtio_network[ module_virtio_network_limit ].pci.bus		= b;
				module_virtio_network[ module_virtio_network_limit ].pci.device		= d;
				module_virtio_network[ module_virtio_network_limit ].pci.function	= f;

				// retrieve class-subclass
				uint32_t device_vendor = driver_pci_read( module_virtio_network[ module_virtio_network_limit ].pci, DRIVER_PCI_REGISTER_vendor_and_device );

				// VirtIO device?
				if( (uint16_t) device_vendor != DRIVER_PCI_DEVICE_VENDOR_virtio ) continue;	// no
				
				// retrieve subsystem id
				module_virtio_network[ module_virtio_network_limit ].subsystem_id = driver_pci_read( module_virtio_network[ module_virtio_network_limit ].pci, DRIVER_PCI_REGISTER_subsystem_and_vendor_id ) >> STD_MOVE_WORD;

				// network adapter?
				if( module_virtio_network[ module_virtio_network_limit ].subsystem_id != MODULE_VIRTIO_SUBSYSTEM_ID_NETWORK ) continue;	// no

				// register Network Controller
				module_virtio_network[ module_virtio_network_limit ].base_address = driver_pci_read( module_virtio_network[ module_virtio_network_limit ].pci, DRIVER_PCI_REGISTER_bar0 );
				module_virtio_network[ module_virtio_network_limit ].mmio_semaphore = FALSE;

				// MMIO type of address?
				if( ! (module_virtio_network[ module_virtio_network_limit ].base_address & TRUE) ) {
					// yes
					module_virtio_network[ module_virtio_network_limit ].mmio_semaphore = TRUE;

					// 64 bit address?
					if( (module_virtio_network[ module_virtio_network_limit ].base_address & ~STD_PAGE_mask) == 0x04 )
						// retrieve higher address value
						module_virtio_network[ module_virtio_network_limit ].base_address |= (uint64_t) driver_pci_read( module_virtio_network[ module_virtio_network_limit ].pci, DRIVER_PCI_REGISTER_bar1 ) << STD_MOVE_DWORD;

					// map MMIO controller area
					kernel -> page_map( kernel -> page_base_address, module_virtio_network[ module_virtio_network_limit ].base_address & STD_PAGE_mask, (module_virtio_network[ module_virtio_network_limit ].base_address & STD_PAGE_mask) | KERNEL_PAGE_mirror, MACRO_PAGE_ALIGN_UP( TRUE ) >> STD_SHIFT_PAGE, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write );

					// debug
					kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u - Network Controller MMIO address 0x%16X\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, module_virtio_network[ module_virtio_network_limit ].base_address &= ~0b00001111 );
				} else
					// debug
					kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u - Network Controller I/O address 0x%X\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, module_virtio_network[ module_virtio_network_limit ].base_address &= ~0b00001111 );

				// properties of device features and status
				uint64_t device_features;
				uint8_t device_status;

				//----------------------------------------------

				// reset device
				device_status = MODULE_VIRTIO_DEVICE_STATUS_failed;
				driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

				// wait for finish
				while( driver_port_in_dword( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_features ) & MODULE_VIRTIO_DEVICE_STATUS_failed );

				//----------------------------------------------

				// device recognized
				device_status = MODULE_VIRTIO_DEVICE_STATUS_acknowledge;
				driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

				// driver available
				device_status |= MODULE_VIRTIO_DEVICE_STATUS_driver_available;
				driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

				//----------------------------------------------

				// retrieve device features
				device_features = driver_port_in_dword( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_features );

				// debug
				kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u - Features: %32b\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, device_features );

				// MAC field?
				if( ! (device_features & MODULE_VIRTIO_DEVICE_FEATURE_mac) ) {
					// set own MAC address
					module_virtio_network[ module_virtio_network_limit ].mac[ 0 ] = 0x00; driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 0 ] ), module_virtio_network[ module_virtio_network_limit ].mac[ 0 ] );
					module_virtio_network[ module_virtio_network_limit ].mac[ 1 ] = 0x22; driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 1 ] ), module_virtio_network[ module_virtio_network_limit ].mac[ 1 ] );
					module_virtio_network[ module_virtio_network_limit ].mac[ 2 ] = 0x44; driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 2 ] ), module_virtio_network[ module_virtio_network_limit ].mac[ 2 ] );
					module_virtio_network[ module_virtio_network_limit ].mac[ 3 ] = 0x66; driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 3 ] ), module_virtio_network[ module_virtio_network_limit ].mac[ 3 ] );
					module_virtio_network[ module_virtio_network_limit ].mac[ 4 ] = 0x88; driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 4 ] ), module_virtio_network[ module_virtio_network_limit ].mac[ 4 ] );
					module_virtio_network[ module_virtio_network_limit ].mac[ 5 ] = 0xAA; driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 5 ] ), module_virtio_network[ module_virtio_network_limit ].mac[ 5 ] );
				}
				
				// Status field?
				if( ! (device_features & MODULE_VIRTIO_DEVICE_FEATURE_status) ) {
					// debug
					kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u - No Status.\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function );

					// no support
					continue;
				}

				//----------------------------------------------

				// inform about supported features by driver
				uint32_t quest_features = MODULE_VIRTIO_DEVICE_FEATURE_mac | MODULE_VIRTIO_DEVICE_FEATURE_status;
				driver_port_out_dword( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_guest_features, device_features );

				//----------------------------------------------

				// select 0th queue
				driver_port_out_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_select, FALSE );

				// register receive queue
				uint64_t queue_receive_limit = driver_port_in_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_limit );
				uintptr_t queue_receive_address = kernel -> memory_alloc_low( MACRO_PAGE_ALIGN_UP( queue_receive_limit * queue_receive_limit ) >> STD_SHIFT_PAGE );
				driver_port_out_dword( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_address, queue_receive_address >> STD_SHIFT_PAGE );

				// select 1st queue
				driver_port_out_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_select, TRUE );

				// register transmit queue
				uint64_t queue_transmit_limit = driver_port_in_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_limit );
				uintptr_t queue_transmit_address = kernel -> memory_alloc_low( MACRO_PAGE_ALIGN_UP( queue_transmit_limit * queue_transmit_limit ) >> STD_SHIFT_PAGE );
				driver_port_out_dword( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_address, queue_transmit_address >> STD_SHIFT_PAGE );

				//----------------------------------------------

				// retrieve MAC address
				module_virtio_network[ module_virtio_network_limit ].mac[ 0 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 0 ] ) );
				module_virtio_network[ module_virtio_network_limit ].mac[ 1 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 1 ] ) );
				module_virtio_network[ module_virtio_network_limit ].mac[ 2 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 2 ] ) );
				module_virtio_network[ module_virtio_network_limit ].mac[ 3 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 3 ] ) );
				module_virtio_network[ module_virtio_network_limit ].mac[ 4 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 4 ] ) );
				module_virtio_network[ module_virtio_network_limit ].mac[ 5 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 5 ] ) );

				// debug
				kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u - MAC address: %2X:%2X:%2X:%2X:%2X:%2X\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, module_virtio_network[ module_virtio_network_limit ].mac[ 0 ], module_virtio_network[ module_virtio_network_limit ].mac[ 1 ], module_virtio_network[ module_virtio_network_limit ].mac[ 2 ], module_virtio_network[ module_virtio_network_limit ].mac[ 3 ], module_virtio_network[ module_virtio_network_limit ].mac[ 4 ], module_virtio_network[ module_virtio_network_limit ].mac[ 5 ] );
			}

	// infinite loop :)
	while( TRUE ) __asm__ volatile( "hlt" );
}
