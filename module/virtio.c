/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	#include	"../library/integer.h"
	#include	"../library/integer.c"

	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel
	//----------------------------------------------------------------------
	#include	"../kernel/config.h"
	#include	"../kernel/idt.h"
	#include	"../kernel/io_apic.h"
	#include	"../kernel/lapic.h"
	#include	"../kernel/page.h"
	#include	"../kernel/storage.h"
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

__attribute__(( preserve_most ))
void module_virtio_irq( void ) {
	// only first device
	for( uint64_t i = 0; i < module_virtio_limit; i++ ) {
		switch( module_virtio[ i ].type ) {
			// VirtIO-Blk
			case MODULE_VIRTIO_TYPE_block: {
				// properties of network device
				struct MODULE_VIRTIO_BLOCK_STRUCTURE *block = (struct MODULE_VIRTIO_BLOCK_STRUCTURE *) module_virtio[ i ].device;

				// retrieve virtio isr status
				uint8_t isr_status = driver_port_in_byte( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_isr_status );

				// block transffered
				block -> semaphore = TRUE;

				// done
				break;
			}

			// VirtIO-Net
			case MODULE_VIRTIO_TYPE_network: {
				// properties of network device
				struct MODULE_VIRTIO_NETWORK_STRUCTURE *network = (struct MODULE_VIRTIO_NETWORK_STRUCTURE *) module_virtio[ i ].device;

				// retrieve virtio isr status
				uint8_t isr_status = driver_port_in_byte( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_isr_status );

				// interrupt from queue? no
				if( ! (isr_status & TRUE) ) {
					// // link change?
					// if( TRUE & driver_port_in_word( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NETWORK_STRUCTURE_DEVICE_CONFIG, status ) ) )
					// 	// debug
					// 	kernel -> log( (uint8_t *) "[VIRTIO] Link up.\n" );
					// else
					// 	// debug
					// 	kernel -> log( (uint8_t *) "[VIRTIO] Link down.\n" );

					// done
					break;
				}

				//----------------------------------------------------------------------

				// synchronize memory with host
				MACRO_SYNC();

				// rings properties
				uint16_t *ring_available = (uint16_t *) (network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_RX ].driver_address + offsetof( struct MODULE_VIRTIO_STRUCTURE_DRIVER, ring ));
				struct MODULE_VIRTIO_STRUCTURE_RING *ring_used = (struct MODULE_VIRTIO_STRUCTURE_RING *) ((uintptr_t) network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_RX ].device_address + 0x04);

				// try to prevent interrupts
				network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_RX ].device_address -> flags = MODULE_VIRTIO_QUEUE_FLAG_interrupt_no;

				// parse all incomming packets
				while( network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_RX ].device_index != network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_RX ].device_address -> index ) {
					// debug
					// kernel -> log( (uint8_t *) "Rx\n" );

					// calculate ring id
					uint64_t index_used = network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_RX ].device_index % network -> queue_limit[ MODULE_VIRTIO_NETWORK_QUEUE_RX ];

					// alloc area for frame content
					uint8_t *source = (uint8_t *) (network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_RX ].descriptor_address[ ring_used[ index_used ].index ].address | KERNEL_PAGE_mirror) + sizeof( struct MODULE_VIRTIO_NETWORK_STRUCTURE_HEADER );
					uint8_t *target = (uint8_t *) kernel -> memory_alloc( TRUE );
					for( uint64_t f = 0; f < ring_used[ index_used ].length - sizeof( struct MODULE_VIRTIO_NETWORK_STRUCTURE_HEADER ); f++ ) target[ f ] = source[ f ];

					// store frame on network stack
					kernel -> network_rx( (uintptr_t) target | ring_used[ index_used ].length | KERNEL_PAGE_mirror );

					// reset entry
					ring_used[ index_used ].length = EMPTY;

					// next descriptor
					network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_RX ].device_index++;

					// synchronize memory with host
					MACRO_SYNC();

					// add descriptor back to available queue ----------------------

					// start counting from current id
					uint64_t index_available = network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_RX ].driver_address -> index % network -> queue_limit[ MODULE_VIRTIO_NETWORK_QUEUE_RX ];

					// add cache to available ring
					ring_available[ index_available ] = ring_used[ index_used ].index;

					// synchronize memory with host
					MACRO_SYNC();

					// set next available index
					network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_RX ].driver_address -> index++;
							
					// synchronize memory with host
					MACRO_SYNC();

					// inform about updated available queue
					driver_port_out_word( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_queue_notify, MODULE_VIRTIO_NETWORK_QUEUE_RX );
				}

				// enable interrupts
				network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_RX ].device_address -> flags = EMPTY;

				//----------------------------------------------------------------------

				// try to prevent interrupts
				network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_TX ].device_address -> flags = MODULE_VIRTIO_QUEUE_FLAG_interrupt_no;

				// parse all outgoed packets
				while( network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_TX ].device_index != network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_TX ].device_address -> index )
					// next entry
					network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_TX ].device_index++;

				// enable interrupts
				network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_TX ].device_address -> flags = EMPTY;

				//----------------------------------------------------------------------

				// done
				break;
			}
		}

		// synchronize memory with host
		MACRO_SYNC();
	}

	//----------------------------------------------------------------------

	// tell APIC of current logical processor that hardware interrupt was handled, propely
	kernel -> lapic_base_address -> eoi = EMPTY;
}

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

	//----------------------------------------------------------------------

	// connect default interrupt handler
	kernel -> idt_mount( KERNEL_IDT_IRQ_offset + module_virtio[ EMPTY ].irq, KERNEL_IDT_TYPE_irq, (uintptr_t) module_virtio_irq_entry );

	// connect interrupt vector from IDT table to IOAPIC controller
	kernel -> io_apic_connect( KERNEL_IDT_IRQ_offset + module_virtio[ EMPTY ].irq, KERNEL_IO_APIC_iowin + (module_virtio[ EMPTY ].irq * 0x02) );

	//----------------------------------------------------------------------

	// initialize all registered network devices
	uint8_t module_virtio_network_string[] = "virtio-net.ao";
	kernel -> module_thread( (uintptr_t) &module_virtio_network, (uint8_t *) &module_virtio_network_string, sizeof( module_virtio_network_string ) - 1 );

	// initialize all registered block devices
	uint8_t module_virtio_block_string[] = "virtio-blk.ao";
	kernel -> module_thread( (uintptr_t) &module_virtio_block, (uint8_t *) &module_virtio_block_string, sizeof( module_virtio_block_string ) - 1 );

	// hodor
	while( TRUE ) kernel -> time_sleep( TRUE );	// release AP time
}
