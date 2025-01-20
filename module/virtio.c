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
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./virtio/data.c"

__attribute__(( preserve_most ))
void module_virtio_net( void ) {
	// retrieve virtio-net isr status
	uint8_t isr_status = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_isr_status );

	// interrupt from queue? no
	if( ! (isr_status & TRUE) ) { kernel -> lapic_base_address -> eoi = EMPTY; return; }

	// synchronize memory with host
	MACRO_SYNC();

	// rings properties
	uint16_t *available = (uint16_t *) (module_virtio_network[ module_virtio_network_limit ].queue[ FALSE ].available_address + offsetof( struct MODULE_VIRTIO_STRUCTURE_AVAILABLE, ring ));
	struct MODULE_VIRTIO_STRUCTURE_RING *used = (struct MODULE_VIRTIO_STRUCTURE_RING *) ((uintptr_t) module_virtio_network[ module_virtio_network_limit ].queue[ FALSE ].used_address + 0x04);

	// parse all incomming packets
	while( module_virtio_network[ module_virtio_network_limit ].queue[ FALSE ].used_index != module_virtio_network[ module_virtio_network_limit ].queue[ FALSE ].used_address -> index ) {
		// calculate ring id
		uint64_t index = module_virtio_network[ module_virtio_network_limit ].queue[ FALSE ].used_index % module_virtio_network[ module_virtio_network_limit ].queue_limit[ FALSE ];

		// return cache back to pool
		module_virtio_network[ module_virtio_network_limit ].queue[ FALSE ].cache_address[ index ].limit = STD_PAGE_byte;
		module_virtio_network[ module_virtio_network_limit ].queue[ FALSE ].cache_address[ index ].flags = MODULE_VIRTIO_NET_CACHE_FLAG_write;

		// debug
		kernel -> log( (uint8_t *) "parsed -> idx %u\n", module_virtio_network[ module_virtio_network_limit ].queue[ FALSE ].used_index );

		// next descriptor
		module_virtio_network[ module_virtio_network_limit ].queue[ FALSE ].used_index++;

		// synchronize memory with host
		MACRO_SYNC();
	}

	// ring defalted?
	if( ! (module_virtio_network[ module_virtio_network_limit ].queue[ FALSE ].used_address -> index % module_virtio_network[ module_virtio_network_limit ].queue_limit[ FALSE ]) ) {
		// synchronize memory with host
		MACRO_SYNC();

		// start counting from current id
		uint16_t current = module_virtio_network[ module_virtio_network_limit ].queue[ FALSE ].used_index;

		// populate new availaable caches
		for( uint64_t i = 0; i < module_virtio_network[ module_virtio_network_limit ].queue_limit[ FALSE ]; i++ ) {
			// add cache to available ring
			available[ i ] = i;

			// synchronize memory with host
			MACRO_SYNC();

			// set next available index
			module_virtio_network[ module_virtio_network_limit ].queue[ FALSE ].available_address -> index++;
				
			// synchronize memory with host
			MACRO_SYNC();

			// inform about updated available shared caches
			// driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_notify, i );
		}
	}

	// tell APIC of current logical processor that hardware interrupt was handled, propely
	kernel -> lapic_base_address -> eoi = EMPTY;
}

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

				// retrieve interrupt number of device
				module_virtio_network[ module_virtio_network_limit ].irq = (uint8_t) driver_pci_read( module_virtio_network[ module_virtio_network_limit ].pci, DRIVER_PCI_REGISTER_irq );

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
					kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u Network Controller MMIO address 0x%16X, IRQ 0x%2X\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, module_virtio_network[ module_virtio_network_limit ].base_address &= ~0b00001111, module_virtio_network[ module_virtio_network_limit ].irq );
				} else
					// debug
					kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u Network Controller I/O address 0x%X, IRQ 0x%2X\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, module_virtio_network[ module_virtio_network_limit ].base_address &= ~0b00001111, module_virtio_network[ module_virtio_network_limit ].irq );

				// properties of device features and status
				uint64_t device_features;
				uint8_t device_status;

				//----------------------------------------------

				// reset device
				device_status = EMPTY;
				driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

				//----------------------------------------------

				// device recognized
				device_status |= MODULE_VIRTIO_DEVICE_STATUS_acknowledge;
				driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

				// driver available
				device_status |= MODULE_VIRTIO_DEVICE_STATUS_driver_available;
				driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

				//----------------------------------------------

				// retrieve device features
				device_features = driver_port_in_dword( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_features );

				// debug
				kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u Features available: %32b\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, device_features );

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
					kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u No Status.\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function );

					// no support
					continue;
				}

				//----------------------------------------------

				// inform about supported features by driver
				uint32_t quest_features = MODULE_VIRTIO_DEVICE_FEATURE_mac | MODULE_VIRTIO_DEVICE_FEATURE_status;
				driver_port_out_dword( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_guest_features, device_features );

				// debug
				kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u Features requested: %32b\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, quest_features );

				// close negotiations
				device_status |= MODULE_VIRTIO_DEVICE_STATUS_features_ok;
				driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

				// retrieve current device status
				device_status = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_status );

				// debug
				kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u Device status: %8b, ", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, device_status );

				// requested features, accepted?
				if( device_status & MODULE_VIRTIO_DEVICE_STATUS_features_ok ) kernel -> log( (uint8_t *) "features accepted.\n" );
				else {
					// debug
					kernel -> log( (uint8_t *) ", ERROR.\n" );

					// leave device
					continue;
				}

				//----------------------------------------------

				// network adapter have 2 queues
				for( uint16_t i = 0; i < 2; i++ ) {
					// select queue
					driver_port_out_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_select, i );

					// check if queue exist
					module_virtio_network[ module_virtio_network_limit ].queue_limit[ i ] = driver_port_in_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_limit );
					if( ! module_virtio_network[ module_virtio_network_limit ].queue_limit[ i ] ) continue;	// doesn't

					// rings sizes
					uint64_t limit_cache = MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_VIRTIO_STRUCTURE_CACHE ) * module_virtio_network[ module_virtio_network_limit ].queue_limit[ i ] );
					uint64_t limit_available = MACRO_PAGE_ALIGN_UP( (sizeof( uint16_t ) * module_virtio_network[ module_virtio_network_limit ].queue_limit[ i ]) + (3 * sizeof( uint16_t )) );
					uint64_t limit_used = MACRO_PAGE_ALIGN_UP( (sizeof( struct MODULE_VIRTIO_STRUCTURE_RING ) * module_virtio_network[ module_virtio_network_limit ].queue_limit[ i ]) + (3 * sizeof( uint16_t )) );

					// acquire area for queue
					module_virtio_network[ module_virtio_network_limit ].queue[ i ].cache_address = (struct MODULE_VIRTIO_STRUCTURE_CACHE *) (kernel -> memory_alloc_low( (limit_cache + limit_available + limit_used) >> STD_SHIFT_PAGE ) | KERNEL_PAGE_mirror);
					module_virtio_network[ module_virtio_network_limit ].queue[ i ].available_address = (struct MODULE_VIRTIO_STRUCTURE_AVAILABLE *) ((uintptr_t) module_virtio_network[ module_virtio_network_limit ].queue[ i ].cache_address + limit_cache);
					module_virtio_network[ module_virtio_network_limit ].queue[ i ].used_address = (struct MODULE_VIRTIO_STRUCTURE_USED *) ((uintptr_t) module_virtio_network[ module_virtio_network_limit ].queue[ i ].cache_address + limit_cache + limit_available);

					// register queue
					driver_port_out_dword( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_address, ((uintptr_t) module_virtio_network[ module_virtio_network_limit ].queue[ i ].cache_address & ~KERNEL_PAGE_mirror) >> STD_SHIFT_PAGE );

					// debug
					kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u Queue[ %u ] at 0x%16X-0x%16X (Cache: 0x%X, Available: 0x%X, Used: 0x%X)\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, i, (uintptr_t) module_virtio_network[ module_virtio_network_limit ].queue[ i ].cache_address, ((uintptr_t) module_virtio_network[ module_virtio_network_limit ].queue[ i ].cache_address + (limit_cache + limit_available + limit_used)) - 1, (uintptr_t) module_virtio_network[ module_virtio_network_limit ].queue[ i ].cache_address & ~KERNEL_PAGE_mirror, ((uintptr_t) module_virtio_network[ module_virtio_network_limit ].queue[ i ].cache_address + limit_cache) & ~KERNEL_PAGE_mirror, ((uintptr_t) module_virtio_network[ module_virtio_network_limit ].queue[ i ].cache_address + limit_cache + limit_available) & ~KERNEL_PAGE_mirror );
				}

				//----------------------------------------------

				// retrieve MAC address
				module_virtio_network[ module_virtio_network_limit ].mac[ 0 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 0 ] ) );
				module_virtio_network[ module_virtio_network_limit ].mac[ 1 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 1 ] ) );
				module_virtio_network[ module_virtio_network_limit ].mac[ 2 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 2 ] ) );
				module_virtio_network[ module_virtio_network_limit ].mac[ 3 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 3 ] ) );
				module_virtio_network[ module_virtio_network_limit ].mac[ 4 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 4 ] ) );
				module_virtio_network[ module_virtio_network_limit ].mac[ 5 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 5 ] ) );

				// debug
				kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u MAC address: %2X:%2X:%2X:%2X:%2X:%2X\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, module_virtio_network[ module_virtio_network_limit ].mac[ 0 ], module_virtio_network[ module_virtio_network_limit ].mac[ 1 ], module_virtio_network[ module_virtio_network_limit ].mac[ 2 ], module_virtio_network[ module_virtio_network_limit ].mac[ 3 ], module_virtio_network[ module_virtio_network_limit ].mac[ 4 ], module_virtio_network[ module_virtio_network_limit ].mac[ 5 ] );

				//----------------------------------------------

				// IRQ line available?
				if( kernel -> io_apic_line( module_virtio_network[ module_virtio_network_limit ].irq ) ) {
					// debug
					kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u IRQ 0x%2X already in use!\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, module_virtio_network[ module_virtio_network_limit ].irq );

					// no support
					continue;
				}

				// connect network controller interrupt handler
				kernel -> idt_mount( KERNEL_IDT_IRQ_offset + module_virtio_network[ module_virtio_network_limit ].irq, KERNEL_IDT_TYPE_irq, (uintptr_t) module_virtio_net_entry );

				// connect interrupt vector from IDT table to IOAPIC controller
				kernel -> io_apic_connect( KERNEL_IDT_IRQ_offset + module_virtio_network[ module_virtio_network_limit ].irq, KERNEL_IO_APIC_iowin + (module_virtio_network[ module_virtio_network_limit ].irq * 0x02) );

				// debug
				kernel -> log( (uint8_t *) "[VIRTIO] IRQ 0x%2X, connected.\n", module_virtio_network[ module_virtio_network_limit ].irq );

				//----------------------------------------------

				// driver configured
				device_status |= MODULE_VIRTIO_DEVICE_STATUS_driver_ok;
				driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

				//----------------------------------------------
				// DEBUG
				//----------------------------------------------

				// transmit function ready?
				// while( ! kernel -> network_tx );

				// hold the door
				// while( TRUE ) {
					// properties of frame to send
					// uintptr_t frame = EMPTY;

					// acquire data for transmission
					// while( ! (frame = kernel -> network_tx()) );

					// resolve properties
					// uint8_t *data = (uint8_t *) (frame & STD_PAGE_mask);
					// uint64_t length = frame & ~STD_PAGE_mask;

					uint64_t queue = MODULE_VIRTIO_QUEUE_RECEIVE;
					uint16_t *ring_available = (uint16_t *) (module_virtio_network[ module_virtio_network_limit ].queue[ queue ].available_address + offsetof( struct MODULE_VIRTIO_STRUCTURE_AVAILABLE, ring ));

					// move packet content behind header
					// for( uint64_t i = length - 1; i > sizeof( struct MODULE_VIRTIO_NET_STRUCTURE_HEADER ); i-- ) data[ i ] = data[ i - sizeof( struct MODULE_VIRTIO_NET_STRUCTURE_HEADER ) ];

					kernel -> log( (uint8_t *) "%u\n", module_virtio_network[ module_virtio_network_limit ].queue_limit[ queue ] );

					// debug
					for( uint64_t i = 0; i < module_virtio_network[ module_virtio_network_limit ].queue_limit[ queue ]; i++ ) {
						// allocate area for cache
						module_virtio_network[ module_virtio_network_limit ].queue[ queue ].cache_address[ i ].address = (uintptr_t) kernel -> memory_alloc_page();
						module_virtio_network[ module_virtio_network_limit ].queue[ queue ].cache_address[ i ].limit = STD_PAGE_byte;
						module_virtio_network[ module_virtio_network_limit ].queue[ queue ].cache_address[ i ].flags = MODULE_VIRTIO_NET_CACHE_FLAG_write;

						// add cache to available ring
						ring_available[ i ] = i;

						// synchronize memory with host
						MACRO_SYNC();

						// set next available index
						module_virtio_network[ module_virtio_network_limit ].queue[ queue ].available_address -> index++;
						
						// synchronize memory with host
						MACRO_SYNC();

						// inform about updated available  shared caches
						// driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_notify, i );
					}

					// retrieve current device status
					device_status = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_status );
					if( device_status & MODULE_VIRTIO_DEVICE_STATUS_device_needs_reset ) {
						// debug
						kernel -> log( (uint8_t *) "ERROR!\n" );
					}
				// }
			}

	// infinite loop :)
	while( TRUE ) __asm__ volatile( "hlt" );
}
