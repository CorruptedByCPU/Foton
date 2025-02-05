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
	#include	"./virtio/config.h"
	#include	"./virtio/network/config.h"
	#include	"./virtio/block/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./virtio/data.c"
	#include	"./virtio/network/data.c"
	#include	"./virtio/block/data.c"
	//----------------------------------------------------------------------
	#include	"./virtio/block.c"
	#include	"./virtio/network.c"
	//----------------------------------------------------------------------

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// initialize VirtIO Network properties
	module_virtio = (struct MODULE_VIRTIO_STRUCTURE *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_VIRTIO_STRUCTURE ) * TRUE ) >> STD_SHIFT_PAGE );

	// check every bus;device;function of PCI controller
	for( uint16_t b = 0; b < 256; b++ )
		for( uint8_t d = 0; d < 32; d++ )
			for( uint8_t f = 0; f < 8; f++ ) {
				//----------------------------------------------

				// PCI properties
				module_virtio[ module_virtio_limit ].pci.result		= EMPTY;
				module_virtio[ module_virtio_limit ].pci.bus		= b;
				module_virtio[ module_virtio_limit ].pci.device		= d;
				module_virtio[ module_virtio_limit ].pci.function	= f;

				// retrieve device-vendor properties
				uint32_t device_vendor = driver_pci_read( module_virtio[ module_virtio_limit ].pci, DRIVER_PCI_REGISTER_vendor_and_device );

				// VirtIO controller?
				if( (uint16_t) device_vendor != DRIVER_PCI_DEVICE_VENDOR_virtio ) continue;	// no
				
				// Supported VirtIO device?
				if( ((device_vendor >> STD_MOVE_WORD) < 0x1000) && ((device_vendor >> STD_MOVE_WORD) > 0x107F) ) continue;	// no

				// retrieve base address
				module_virtio[ module_virtio_limit ].base_address = driver_pci_read( module_virtio[ module_virtio_limit ].pci, DRIVER_PCI_REGISTER_bar0 ) & 0xFFF0;

				// and IRQ number
				module_virtio[ module_virtio_limit ].irq = (uint8_t) driver_pci_read( module_virtio[ module_virtio_limit ].pci, DRIVER_PCI_REGISTER_irq );

				// by default Legacy device
				module_virtio[ module_virtio_limit ].semaphore_legacy = TRUE;

				// Modern device?
				if( ((device_vendor >> STD_MOVE_WORD) > 0x103F) && ((device_vendor >> STD_MOVE_WORD) < 0x1080) || ((uint8_t) driver_pci_read( module_virtio[ module_virtio_limit ].pci, DRIVER_PCI_REGISTER_class_and_subclass ) > 0) || ((driver_pci_read( module_virtio[ module_virtio_limit ].pci, DRIVER_PCI_REGISTER_subsystem_and_vendor_id ) >> STD_MOVE_WORD) >= 0x40) ) module_virtio[ module_virtio_limit ].semaphore_modern = TRUE;	// yes

				// Transitional device?
				if( (device_vendor >> STD_MOVE_WORD) > 0x0FFF && (device_vendor >> STD_MOVE_WORD) < 0x1040 && (uint8_t) driver_pci_read( module_virtio[ module_virtio_limit ].pci, DRIVER_PCI_REGISTER_class_and_subclass ) == 0 ) module_virtio[ module_virtio_limit ].semaphore_transitional = TRUE;	// yes

				// check device type

				// for modern device
				if( module_virtio[ module_virtio_limit ].semaphore_modern ) {
					switch( device_vendor >> STD_MOVE_WORD ) {
						case 0x1041: { module_virtio[ module_virtio_limit ].type = MODULE_VIRTIO_TYPE_network; break; }
						case 0x1042: { module_virtio[ module_virtio_limit ].type = MODULE_VIRTIO_TYPE_block; break; }
					}
				// for transitional device
				} else if( module_virtio[ module_virtio_limit ].semaphore_transitional ) {
					switch( driver_pci_read( module_virtio[ module_virtio_limit ].pci, DRIVER_PCI_REGISTER_subsystem_and_vendor_id ) >> STD_MOVE_WORD ) {
						case MODULE_VIRTIO_DEVICE_ID_network: { module_virtio[ module_virtio_limit ].type = MODULE_VIRTIO_TYPE_network; break; }
						case MODULE_VIRTIO_DEVICE_ID_block: { module_virtio[ module_virtio_limit ].type = MODULE_VIRTIO_TYPE_block; break; }
					}
				// for legacy device
				} else {
					switch( device_vendor >> STD_MOVE_WORD ) {
						case MODULE_VIRTIO_DEVICE_network: { module_virtio[ module_virtio_limit ].type = MODULE_VIRTIO_TYPE_network; break; }
						case MODULE_VIRTIO_DEVICE_block: { module_virtio[ module_virtio_limit ].type = MODULE_VIRTIO_TYPE_block; break; }
					}
				}

				// device registered
				module_virtio_limit++;
			}

	// initialize all registered network devices
	uint8_t module_virtio_network_string[] = "virtio-net.ao";
	kernel -> module_thread( (uintptr_t) &module_virtio_network, (uint8_t *) &module_virtio_network_string, sizeof( module_virtio_network_string ) - 1 );

	// initialize all registered block devices
	uint8_t module_virtio_block_string[] = "virtio-blk.ao";
	kernel -> module_thread( (uintptr_t) &module_virtio_block, (uint8_t *) &module_virtio_block_string, sizeof( module_virtio_block_string ) - 1 );

	// hodor
	while( TRUE ) kernel -> time_sleep( TRUE );	// release AP time
}
