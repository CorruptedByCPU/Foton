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

				driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_status, TRUE );
				driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_status, 3 );
				uintptr_t queue_receive = kernel -> memory_alloc_low( TRUE );
				driver_port_out_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_select, FALSE );
				kernel -> log( (uint8_t *) "0x%X\n", driver_port_in_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_limit ) );
				driver_port_out_dword( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_address, queue_receive >> STD_SHIFT_PAGE );
				uintptr_t queue_transmit = kernel -> memory_alloc_low( TRUE );
				driver_port_out_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_select, TRUE );
				driver_port_out_dword( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_address, queue_transmit >> STD_SHIFT_PAGE );

				driver_port_out_dword( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_guest_features, 0x00010020 );

				// driver_port_out_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mtu ), 1280 );

				uint8_t mac[ 6 ];
				mac[ 0 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 0 ] ) );
				mac[ 1 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 1 ] ) );
				mac[ 2 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 2 ] ) );
				mac[ 3 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 3 ] ) );
				mac[ 4 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 4 ] ) );
				mac[ 5 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 5 ] ) );
				kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u - MAC address: %2X:%2X:%2X:%2X:%2X:%2X, MTU: %u\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, mac[ 0 ], mac[ 1 ], mac[ 2 ], mac[ 3 ], mac[ 4 ], mac[ 5 ], driver_port_in_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mtu ) ) );

				kernel -> log( (uint8_t *) "%b 0x%X\n", driver_port_in_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, status ) ), driver_port_in_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_select ) );
			}

	// infinite loop :)
	while( TRUE ) __asm__ volatile( "hlt" );
}
