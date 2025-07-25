/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel
	//----------------------------------------------------------------------
	#include	"../kernel/idt.h"
	// #include	"../kernel/io_apic.h"
	// #include	"../kernel/apic.h"
	#include	"../kernel/config.h"
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

	// initialize VirtIO device properties
	module_virtio = (struct MODULE_VIRTIO_STRUCTURE *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_VIRTIO_STRUCTURE ) ) >> STD_SHIFT_PAGE );

	// check every bus;device;function of PCI controller
	for( uint16_t b = 0; b < 256; b++ )
		for( uint8_t d = 0; d < 32; d++ )
			for( uint8_t f = 0; f < 8; f++ ) {
				// PCI properties
				module_virtio[ module_virtio_limit ].pci.result		= EMPTY;
				module_virtio[ module_virtio_limit ].pci.bus		= b;
				module_virtio[ module_virtio_limit ].pci.device		= d;
				module_virtio[ module_virtio_limit ].pci.function	= f;

				// retrieve device-vendor properties
				uint32_t device_vendor = driver_pci_read( module_virtio[ module_virtio_limit ].pci, DRIVER_PCI_REGISTER_vendor_and_device );

				// VirtIO controller?
				if( (uint16_t) device_vendor != DRIVER_PCI_DEVICE_VENDOR_virtio ) continue;	// no

				// debug
				kernel -> log( (uint8_t *) "[VirtIO] Controller found at %2X:%2X:%u\n", module_virtio[ module_virtio_limit ].pci.bus, module_virtio[ module_virtio_limit ].pci.device, module_virtio[ module_virtio_limit ].pci.function );
			}

	// hodor
	while( TRUE ) kernel -> time_sleep( TRUE );	// release AP time
}
