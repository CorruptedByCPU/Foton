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
	if( ! (isr_status & TRUE) ) {
		// // link change?
		// if( TRUE & driver_port_in_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, status ) ) )
		// 	// debug
		// 	kernel -> log( (uint8_t *) "[VIRTIO] Link up.\n" );
		// else
		// 	// debug
		// 	kernel -> log( (uint8_t *) "[VIRTIO] Link down.\n" );

		// tell APIC of current logical processor that hardware interrupt was handled, propely
		kernel -> lapic_base_address -> eoi = EMPTY;

		// done
		return;
	}

	//----------------------------------------------------------------------

	// synchronize memory with host
	MACRO_SYNC();

	// rings properties
	uint16_t *ring_available = (uint16_t *) (module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_RX ].available_address + offsetof( struct MODULE_VIRTIO_STRUCTURE_AVAILABLE, ring ));
	struct MODULE_VIRTIO_STRUCTURE_RING *ring_used = (struct MODULE_VIRTIO_STRUCTURE_RING *) ((uintptr_t) module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_RX ].used_address + 0x04);

	// try to prevent interrupts
	module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_RX ].used_address -> flags = MODULE_VIRTIO_NET_QUEUE_FLAG_interrupt_no;

	// parse all incomming packets
	while( module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_RX ].used_index != module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_RX ].used_address -> index ) {
		// calculate ring id
		uint64_t index_used = module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_RX ].used_index % module_virtio_network[ module_virtio_network_limit ].queue_limit[ MODULE_VIRTIO_NET_QUEUE_RX ];

		// alloc area for frame content
		uint8_t *source = (uint8_t *) (module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_RX ].descriptor_address[ ring_used[ index_used ].index ].address | KERNEL_PAGE_mirror) + sizeof( struct MODULE_VIRTIO_NET_STRUCTURE_HEADER );
		uint8_t *target = (uint8_t *) kernel -> memory_alloc( TRUE );
		for( uint64_t f = 0; f < ring_used[ index_used ].length - sizeof( struct MODULE_VIRTIO_NET_STRUCTURE_HEADER ); f++ ) target[ f ] = source[ f ];

		// store frame on network stack
		kernel -> network_rx( (uintptr_t) target | ring_used[ index_used ].length | KERNEL_PAGE_mirror );

		// reset entry
		ring_used[ index_used ].length = EMPTY;

		// next descriptor
		module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_RX ].used_index++;

		// synchronize memory with host
		MACRO_SYNC();

		// add descriptor back to available queue ----------------------

		// start counting from current id
		uint64_t index_available = module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_RX ].available_address -> index % module_virtio_network[ module_virtio_network_limit ].queue_limit[ MODULE_VIRTIO_NET_QUEUE_RX ];

		// add cache to available ring
		ring_available[ index_available ] = ring_used[ index_used ].index;

		// synchronize memory with host
		MACRO_SYNC();

		// set next available index
		module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_RX ].available_address -> index++;
				
		// synchronize memory with host
		MACRO_SYNC();

		// inform about updated available queue
		driver_port_out_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_notify, MODULE_VIRTIO_NET_QUEUE_RX );
	}

	// enable interrupts
	module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_RX ].used_address -> flags = EMPTY;

	//----------------------------------------------------------------------

	// try to prevent interrupts
	module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_TX ].used_address -> flags = MODULE_VIRTIO_NET_QUEUE_FLAG_interrupt_no;

	// parse all outgoed packets
	while( module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_TX ].used_index != module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_TX ].used_address -> index )
		// next entry
		module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_TX ].used_index++;

	// enable interrupts
	module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_TX ].used_address -> flags = EMPTY;

	//----------------------------------------------------------------------

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
					// kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u Network Controller MMIO address 0x%16X, IRQ 0x%2X\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, module_virtio_network[ module_virtio_network_limit ].base_address & ~0b00001111, module_virtio_network[ module_virtio_network_limit ].irq );
				} // else
					// debug
					// kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u Network Controller I/O address 0x%X, IRQ 0x%2X\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, module_virtio_network[ module_virtio_network_limit ].base_address & ~0b00001111, module_virtio_network[ module_virtio_network_limit ].irq );

				// remove unused bits
				module_virtio_network[ module_virtio_network_limit ].base_address &= ~0b00001111;

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
				// kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u Features available: %32b\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, device_features );

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
					// kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u No Status.\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function );

					// no support
					continue;
				}

				//----------------------------------------------

				// inform about supported features by driver
				uint32_t quest_features = MODULE_VIRTIO_DEVICE_FEATURE_mac | MODULE_VIRTIO_DEVICE_FEATURE_status;
				driver_port_out_dword( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_guest_features, device_features );

				// debug
				// kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u Features requested: %32b\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, quest_features );

				// close negotiations
				device_status |= MODULE_VIRTIO_DEVICE_STATUS_features_ok;
				driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

				// retrieve current device status
				device_status = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_status );

				// debug
				// kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u Device status: %8b, ", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, device_status );

				// // requested features, accepted?
				// if( device_status & MODULE_VIRTIO_DEVICE_STATUS_features_ok ) kernel -> log( (uint8_t *) "features accepted.\n" );
				// else {
				// 	// debug
				// 	kernel -> log( (uint8_t *) ", ERROR.\n" );

				// 	// leave device
				// 	continue;
				// }

				//----------------------------------------------

				// network adapter have 2 queues
				for( uint16_t i = 0; i < 2; i++ ) {
					// select queue
					driver_port_out_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_select, i );

					// check if queue exist
					module_virtio_network[ module_virtio_network_limit ].queue_limit[ i ] = driver_port_in_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_limit );
					if( ! module_virtio_network[ module_virtio_network_limit ].queue_limit[ i ] ) continue;	// doesn't

					// rings sizes
					uint64_t limit_cache = MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_VIRTIO_STRUCTURE_DESCRIPTOR ) * module_virtio_network[ module_virtio_network_limit ].queue_limit[ i ] );
					uint64_t limit_available = MACRO_PAGE_ALIGN_UP( (sizeof( uint16_t ) * module_virtio_network[ module_virtio_network_limit ].queue_limit[ i ]) + (3 * sizeof( uint16_t )) );
					uint64_t limit_used = MACRO_PAGE_ALIGN_UP( (sizeof( struct MODULE_VIRTIO_STRUCTURE_RING ) * module_virtio_network[ module_virtio_network_limit ].queue_limit[ i ]) + (3 * sizeof( uint16_t )) );

					// acquire area for queue
					module_virtio_network[ module_virtio_network_limit ].queue[ i ].descriptor_address = (struct MODULE_VIRTIO_STRUCTURE_DESCRIPTOR *) (kernel -> memory_alloc_low( (limit_cache + limit_available + limit_used) >> STD_SHIFT_PAGE ) | KERNEL_PAGE_mirror);
					module_virtio_network[ module_virtio_network_limit ].queue[ i ].available_address = (struct MODULE_VIRTIO_STRUCTURE_AVAILABLE *) ((uintptr_t) module_virtio_network[ module_virtio_network_limit ].queue[ i ].descriptor_address + limit_cache);
					module_virtio_network[ module_virtio_network_limit ].queue[ i ].used_address = (struct MODULE_VIRTIO_STRUCTURE_USED *) ((uintptr_t) module_virtio_network[ module_virtio_network_limit ].queue[ i ].descriptor_address + limit_cache + limit_available);

					// register queue
					driver_port_out_dword( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_address, ((uintptr_t) module_virtio_network[ module_virtio_network_limit ].queue[ i ].descriptor_address & ~KERNEL_PAGE_mirror) >> STD_SHIFT_PAGE );

					// debug
					// kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u Queue[ %u ] at 0x%16X-0x%16X (Cache: 0x%X, Available: 0x%X, Used: 0x%X)\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, i, (uintptr_t) module_virtio_network[ module_virtio_network_limit ].queue[ i ].descriptor_address, ((uintptr_t) module_virtio_network[ module_virtio_network_limit ].queue[ i ].descriptor_address + (limit_cache + limit_available + limit_used)) - 1, (uintptr_t) module_virtio_network[ module_virtio_network_limit ].queue[ i ].descriptor_address & ~KERNEL_PAGE_mirror, ((uintptr_t) module_virtio_network[ module_virtio_network_limit ].queue[ i ].descriptor_address + limit_cache) & ~KERNEL_PAGE_mirror, ((uintptr_t) module_virtio_network[ module_virtio_network_limit ].queue[ i ].descriptor_address + limit_cache + limit_available) & ~KERNEL_PAGE_mirror );
				}

				//----------------------------------------------

				// retrieve MAC address
				module_virtio_network[ module_virtio_network_limit ].mac[ 0 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 0 ] ) );
				module_virtio_network[ module_virtio_network_limit ].mac[ 1 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 1 ] ) );
				module_virtio_network[ module_virtio_network_limit ].mac[ 2 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 2 ] ) );
				module_virtio_network[ module_virtio_network_limit ].mac[ 3 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 3 ] ) );
				module_virtio_network[ module_virtio_network_limit ].mac[ 4 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 4 ] ) );
				module_virtio_network[ module_virtio_network_limit ].mac[ 5 ] = driver_port_in_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_STRUCTURE_NETWORK_DEVICE_CONFIG, mac[ 5 ] ) );

				// update
				kernel -> network_interface.ethernet_address[ 0 ] = module_virtio_network[ module_virtio_network_limit ].mac[ 0 ];
				kernel -> network_interface.ethernet_address[ 1 ] = module_virtio_network[ module_virtio_network_limit ].mac[ 1 ];
				kernel -> network_interface.ethernet_address[ 2 ] = module_virtio_network[ module_virtio_network_limit ].mac[ 2 ];
				kernel -> network_interface.ethernet_address[ 3 ] = module_virtio_network[ module_virtio_network_limit ].mac[ 3 ];
				kernel -> network_interface.ethernet_address[ 4 ] = module_virtio_network[ module_virtio_network_limit ].mac[ 4 ];
				kernel -> network_interface.ethernet_address[ 5 ] = module_virtio_network[ module_virtio_network_limit ].mac[ 5 ];

				// debug
				// kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u MAC address: %2X:%2X:%2X:%2X:%2X:%2X\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, module_virtio_network[ module_virtio_network_limit ].mac[ 0 ], module_virtio_network[ module_virtio_network_limit ].mac[ 1 ], module_virtio_network[ module_virtio_network_limit ].mac[ 2 ], module_virtio_network[ module_virtio_network_limit ].mac[ 3 ], module_virtio_network[ module_virtio_network_limit ].mac[ 4 ], module_virtio_network[ module_virtio_network_limit ].mac[ 5 ] );

				//----------------------------------------------

				// IRQ line available?
				if( kernel -> io_apic_line( module_virtio_network[ module_virtio_network_limit ].irq ) ) {
					// debug
					// kernel -> log( (uint8_t *) "[VIRTIO].%u PCI %2X:%2X.%u IRQ 0x%2X already in use!\n", module_virtio_network_limit, module_virtio_network[ module_virtio_network_limit ].pci.bus, module_virtio_network[ module_virtio_network_limit ].pci.device, module_virtio_network[ module_virtio_network_limit ].pci.function, module_virtio_network[ module_virtio_network_limit ].irq );

					// no support
					continue;
				}

				// connect network controller interrupt handler
				kernel -> idt_mount( KERNEL_IDT_IRQ_offset + module_virtio_network[ module_virtio_network_limit ].irq, KERNEL_IDT_TYPE_irq, (uintptr_t) module_virtio_net_entry );

				// connect interrupt vector from IDT table to IOAPIC controller
				kernel -> io_apic_connect( KERNEL_IDT_IRQ_offset + module_virtio_network[ module_virtio_network_limit ].irq, KERNEL_IO_APIC_iowin + (module_virtio_network[ module_virtio_network_limit ].irq * 0x02) );

				// debug
				// kernel -> log( (uint8_t *) "[VIRTIO] IRQ 0x%2X, connected.\n", module_virtio_network[ module_virtio_network_limit ].irq );

				//----------------------------------------------

				// fill Receive Queue
				for( uint64_t i = 0; i < module_virtio_network[ module_virtio_network_limit ].queue_limit[ MODULE_VIRTIO_NET_QUEUE_RX ]; i++ ) {
					// allocate area in Descriptors Queue
					module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_RX ].descriptor_address[ i ].address = (uintptr_t) kernel -> memory_alloc_page();
					module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_RX ].descriptor_address[ i ].limit = STD_PAGE_byte;
					module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_RX ].descriptor_address[ i ].flags = MODULE_VIRTIO_NET_CACHE_FLAG_write_only;

					// add cache to available ring
					uint16_t *receive_ring_available = (uint16_t *) (module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_RX ].available_address + offsetof( struct MODULE_VIRTIO_STRUCTURE_AVAILABLE, ring ));
					receive_ring_available[ i ] = i;

					// synchronize memory with host
					MACRO_SYNC();

					// set next available index
					module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_RX ].available_address -> index++;

					// synchronize memory with host
					MACRO_SYNC();
				}

				// inform about updated Receive Queue
				driver_port_out_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_notify, MODULE_VIRTIO_NET_QUEUE_RX );

				// fill Transmit Queue
				for( uint64_t i = 0; i < module_virtio_network[ module_virtio_network_limit ].queue_limit[ MODULE_VIRTIO_NET_QUEUE_TX ]; i++ ) {
					// allocate area in Descriptors Queue
					module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_TX ].descriptor_address[ i ].address = (uintptr_t) kernel -> memory_alloc_page();
					module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_TX ].descriptor_address[ i ].limit = STD_PAGE_byte;
					module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_TX ].descriptor_address[ i ].flags = MODULE_VIRTIO_NET_CACHE_FLAG_read_only;

					// synchronize memory with host
					MACRO_SYNC();
				}

				// inform about updated Transmit Queue
				driver_port_out_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_notify, MODULE_VIRTIO_NET_QUEUE_TX );

				//----------------------------------------------

				// driver configured
				device_status |= MODULE_VIRTIO_DEVICE_STATUS_driver_ok;
				driver_port_out_byte( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

				//----------------------------------------------

				// transmit function ready?
				while( ! kernel -> network_tx );

				// hold the door
				while( TRUE ) {
					// properties of frame to send
					uintptr_t frame = EMPTY;

					// acquire data for transmission
					while( ! (frame = kernel -> network_tx()) );

					// debug
					// kernel -> log( (uint8_t *) "Tx %u\n", module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_TX ].available_address -> index );

					// resolve properties
					uint8_t *data = (uint8_t *) (frame & STD_PAGE_mask | KERNEL_PAGE_mirror);
					uint64_t length = frame & ~STD_PAGE_mask;

					// move packet content behind header
					if( length + sizeof( struct MODULE_VIRTIO_NET_STRUCTURE_HEADER ) >= STD_PAGE_byte ) {
						// debug
						// kernel -> log( (uint8_t *) "[VIRTIO] Tx overflow.\n" );

						// ignore packet
						continue;
					}

					// properties of
					uint16_t index = module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_TX ].available_address -> index % module_virtio_network[ module_virtio_network_limit ].queue_limit[ MODULE_VIRTIO_NET_QUEUE_TX ];
					struct MODULE_VIRTIO_STRUCTURE_DESCRIPTOR *descriptor	= (struct MODULE_VIRTIO_STRUCTURE_DESCRIPTOR *) &module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_TX ].descriptor_address[ index ];
					struct MODULE_VIRTIO_STRUCTURE_AVAILABLE *available	= (struct MODULE_VIRTIO_STRUCTURE_AVAILABLE *) module_virtio_network[ module_virtio_network_limit ].queue[ MODULE_VIRTIO_NET_QUEUE_TX ].available_address;
					uint16_t *available_ring				= (uint16_t *) ((uintptr_t) available + offsetof( struct MODULE_VIRTIO_STRUCTURE_AVAILABLE, ring ));

					// copy packet content to descriptor
					uint8_t *source = (uint8_t *) ((uintptr_t) data | KERNEL_PAGE_mirror);
					uint8_t *target = (uint8_t *) (descriptor -> address | KERNEL_PAGE_mirror);
					for( int64_t i = 0; i < length; i++ ) target[ i + sizeof( struct MODULE_VIRTIO_NET_STRUCTURE_HEADER ) ] = source[ i ];

					// set descriptor
					descriptor -> limit = length + sizeof( struct MODULE_VIRTIO_NET_STRUCTURE_HEADER );

					// prepare header
					struct MODULE_VIRTIO_NET_STRUCTURE_HEADER *header = (struct MODULE_VIRTIO_NET_STRUCTURE_HEADER *) target;
					header -> flags		= EMPTY;
					header -> gso_type	= EMPTY;
					header -> header_length	= sizeof( struct MODULE_VIRTIO_NET_STRUCTURE_HEADER );
					header -> gso_size	= EMPTY;
					header -> csum_start	= EMPTY;
					header -> csum_limit	= EMPTY;
					header -> num_buffers	= EMPTY;

					// debug
					kernel -> log( (uint8_t *) "\n" ); for( uint64_t i = 0; i < (descriptor -> limit % 0x10) + TRUE; i++ ) { kernel -> log( (uint8_t *) "0x%8X", (uintptr_t) target + (i * 0x10) ); for( uint64_t k = i * 0x10; k < ((i * 0x10) + 0x10); k++ ) kernel -> log( (uint8_t *) " %2X", target[ k ] ); kernel -> log( (uint8_t *) "\n" ); }

					// add to available ring
					available_ring[ index ] = index;

					// set next available index
					available -> index++;

					// synchronize memory with host
					MACRO_SYNC();

					// inform about updated available queue
					driver_port_out_word( module_virtio_network[ module_virtio_network_limit ].base_address + MODULE_VIRTIO_REGISTER_queue_notify, MODULE_VIRTIO_NET_QUEUE_TX );

					// release frame
					kernel -> memory_release_page( (uintptr_t) data );
				}
			}

	// hodor
	while( TRUE );
}
