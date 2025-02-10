/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

__attribute__(( preserve_most ))
void module_virtio_network_rx( void ) {
	// only first device
	uint64_t i = 0; for( ; i < module_virtio_limit; i++ ) if( module_virtio[ i ].type == MODULE_VIRTIO_TYPE_network ) break;

	// properties of network device
	struct MODULE_VIRTIO_NETWORK_STRUCTURE *network = (struct MODULE_VIRTIO_NETWORK_STRUCTURE *) module_virtio[ i ].device;

	// retrieve virtio-net isr status
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

		// tell APIC of current logical processor that hardware interrupt was handled, propely
		kernel -> lapic_base_address -> eoi = EMPTY;

		// done
		return;
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

	// tell APIC of current logical processor that hardware interrupt was handled, propely
	kernel -> lapic_base_address -> eoi = EMPTY;
}

void module_virtio_network( void ) {
	// configure only first device
	uint64_t i = 0; for( ; i < module_virtio_limit; i++ ) if( module_virtio[ i ].type == MODULE_VIRTIO_TYPE_network ) break;

	// initialize VirtIO Network properties
	module_virtio[ i ].device = kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_VIRTIO_NETWORK_STRUCTURE ) ) >> STD_SHIFT_PAGE );
	struct MODULE_VIRTIO_NETWORK_STRUCTURE *network = (struct MODULE_VIRTIO_NETWORK_STRUCTURE *) module_virtio[ i ].device;

	// module entry
	network -> id = i;

	// debug
	// kernel -> log( (uint8_t *) "VirtIO-Net.\n" );

	// properties of device features and status
	uint64_t device_features;
	uint8_t device_status;

	//----------------------------------------------------------------------

	// reset device
	device_status = EMPTY;
	driver_port_out_byte( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

	//----------------------------------------------------------------------

	// device recognized
	device_status |= MODULE_VIRTIO_DEVICE_STATUS_acknowledge;
	driver_port_out_byte( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

	// driver available
	device_status |= MODULE_VIRTIO_DEVICE_STATUS_driver_available;
	driver_port_out_byte( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

	//----------------------------------------------------------------------

	// retrieve device features
	device_features = driver_port_in_dword( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_features );

	// MAC field?
	if( ! (device_features & MODULE_VIRTIO_NETWORK_FEATURE_MAC) ) {
		// set own MAC address
		network -> mac[ 0 ] = 0x00; driver_port_out_byte( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NETWORK_STRUCTURE_DEVICE_CONFIG, mac[ 0 ] ), network -> mac[ 0 ] );
		network -> mac[ 1 ] = 0x22; driver_port_out_byte( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NETWORK_STRUCTURE_DEVICE_CONFIG, mac[ 1 ] ), network -> mac[ 1 ] );
		network -> mac[ 2 ] = 0x44; driver_port_out_byte( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NETWORK_STRUCTURE_DEVICE_CONFIG, mac[ 2 ] ), network -> mac[ 2 ] );
		network -> mac[ 3 ] = 0x66; driver_port_out_byte( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NETWORK_STRUCTURE_DEVICE_CONFIG, mac[ 3 ] ), network -> mac[ 3 ] );
		network -> mac[ 4 ] = 0x88; driver_port_out_byte( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NETWORK_STRUCTURE_DEVICE_CONFIG, mac[ 4 ] ), network -> mac[ 4 ] );
		network -> mac[ 5 ] = 0xAA; driver_port_out_byte( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NETWORK_STRUCTURE_DEVICE_CONFIG, mac[ 5 ] ), network -> mac[ 5 ] );
	}
	
	// Status field?
	if( ! (device_features & MODULE_VIRTIO_NETWORK_FEATURE_STATUS) ) return;	// no

	//----------------------------------------------------------------------

	// inform about supported/required features by driver
	uint32_t quest_features = MODULE_VIRTIO_NETWORK_FEATURE_MAC | MODULE_VIRTIO_NETWORK_FEATURE_STATUS;	// do not touch MODULE_VIRTIO_NETWORK_FEATURE_MRG_RXBUF, Qemu ignores it
	driver_port_out_dword( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_guest_features, device_features );

	// close negotiations
	device_status |= MODULE_VIRTIO_DEVICE_STATUS_features_ok;
	driver_port_out_byte( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

	// retrieve current device status
	device_status = driver_port_in_byte( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_status );

	// requested features, accepted?
	if( ! (device_status & MODULE_VIRTIO_DEVICE_STATUS_features_ok) ) return;	// no

	//----------------------------------------------------------------------

	// network adapter have 2 queues
	for( uint16_t i = 0; i < 2; i++ ) {
		// select queue
		driver_port_out_word( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_queue_select, i );

		// check if queue exist
		network -> queue_limit[ i ] = driver_port_in_word( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_queue_limit );
		if( ! network -> queue_limit[ i ] ) continue;	// doesn't

		// rings sizes
		uint64_t limit_cache = MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_VIRTIO_STRUCTURE_DESCRIPTOR ) * network -> queue_limit[ i ] );
		uint64_t limit_available = MACRO_PAGE_ALIGN_UP( (sizeof( uint16_t ) * network -> queue_limit[ i ]) + (3 * sizeof( uint16_t )) );
		uint64_t limit_used = MACRO_PAGE_ALIGN_UP( (sizeof( struct MODULE_VIRTIO_STRUCTURE_RING ) * network -> queue_limit[ i ]) + (3 * sizeof( uint16_t )) );

		// acquire area for queue
		network -> queue[ i ].descriptor_address = (struct MODULE_VIRTIO_STRUCTURE_DESCRIPTOR *) (kernel -> memory_alloc_low( (limit_cache + limit_available + limit_used) >> STD_SHIFT_PAGE ) | KERNEL_PAGE_mirror);
		network -> queue[ i ].driver_address = (struct MODULE_VIRTIO_STRUCTURE_DRIVER *) ((uintptr_t) network -> queue[ i ].descriptor_address + limit_cache);
		network -> queue[ i ].device_address = (struct MODULE_VIRTIO_STRUCTURE_DEVICE *) ((uintptr_t) network -> queue[ i ].descriptor_address + limit_cache + limit_available);

		// register queue
		driver_port_out_dword( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_queue_address, ((uintptr_t) network -> queue[ i ].descriptor_address & ~KERNEL_PAGE_mirror) >> STD_SHIFT_PAGE );
	}

	//----------------------------------------------------------------------

	// retrieve MAC address
	network -> mac[ 0 ] = driver_port_in_byte( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NETWORK_STRUCTURE_DEVICE_CONFIG, mac[ 0 ] ) );
	network -> mac[ 1 ] = driver_port_in_byte( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NETWORK_STRUCTURE_DEVICE_CONFIG, mac[ 1 ] ) );
	network -> mac[ 2 ] = driver_port_in_byte( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NETWORK_STRUCTURE_DEVICE_CONFIG, mac[ 2 ] ) );
	network -> mac[ 3 ] = driver_port_in_byte( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NETWORK_STRUCTURE_DEVICE_CONFIG, mac[ 3 ] ) );
	network -> mac[ 4 ] = driver_port_in_byte( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NETWORK_STRUCTURE_DEVICE_CONFIG, mac[ 4 ] ) );
	network -> mac[ 5 ] = driver_port_in_byte( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_NETWORK_STRUCTURE_DEVICE_CONFIG, mac[ 5 ] ) );

	// update
	kernel -> network_interface.ethernet_address[ 0 ] = network -> mac[ 0 ];
	kernel -> network_interface.ethernet_address[ 1 ] = network -> mac[ 1 ];
	kernel -> network_interface.ethernet_address[ 2 ] = network -> mac[ 2 ];
	kernel -> network_interface.ethernet_address[ 3 ] = network -> mac[ 3 ];
	kernel -> network_interface.ethernet_address[ 4 ] = network -> mac[ 4 ];
	kernel -> network_interface.ethernet_address[ 5 ] = network -> mac[ 5 ];

	//----------------------------------------------------------------------

	// connect network controller interrupt handler
	kernel -> idt_mount( KERNEL_IDT_IRQ_offset + module_virtio[ network -> id ].irq, KERNEL_IDT_TYPE_irq, (uintptr_t) module_virtio_network_entry );

	// connect interrupt vector from IDT table to IOAPIC controller
	kernel -> io_apic_connect( KERNEL_IDT_IRQ_offset + module_virtio[ network -> id ].irq, KERNEL_IO_APIC_iowin + (module_virtio[ network -> id ].irq * 0x02) );

	//----------------------------------------------------------------------

	// fill Receive Queue
	for( uint64_t i = 0; i < network -> queue_limit[ MODULE_VIRTIO_NETWORK_QUEUE_RX ]; i++ ) {
		// allocate area in Descriptors Queue
		network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_RX ].descriptor_address[ i ].address = (uintptr_t) kernel -> memory_alloc_page();
		network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_RX ].descriptor_address[ i ].limit = STD_PAGE_byte;
		network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_RX ].descriptor_address[ i ].flags = MODULE_VIRTIO_DESCRIPTOR_FLAG_WRITE;

		// add cache to available ring
		uint16_t *receive_ring_available = (uint16_t *) (network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_RX ].driver_address + offsetof( struct MODULE_VIRTIO_STRUCTURE_DRIVER, ring ));
		receive_ring_available[ i ] = i;

		// synchronize memory with host
		MACRO_SYNC();

		// set next available index
		network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_RX ].driver_address -> index++;

		// synchronize memory with host
		MACRO_SYNC();
	}

	// inform about updated Receive Queue
	driver_port_out_word( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_queue_notify, MODULE_VIRTIO_NETWORK_QUEUE_RX );

	// fill Transmit Queue
	for( uint64_t i = 0; i < network -> queue_limit[ MODULE_VIRTIO_NETWORK_QUEUE_TX ]; i++ ) {
		// allocate area in Descriptors Queue
		network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_TX ].descriptor_address[ i ].address = (uintptr_t) kernel -> memory_alloc_page();
		network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_TX ].descriptor_address[ i ].limit = STD_PAGE_byte;
		network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_TX ].descriptor_address[ i ].flags = MODULE_VIRTIO_DESCRIPTOR_FLAG_READ;

		// synchronize memory with host
		MACRO_SYNC();
	}

	// inform about updated Transmit Queue
	driver_port_out_word( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_queue_notify, MODULE_VIRTIO_NETWORK_QUEUE_TX );

	//----------------------------------------------------------------------

	// driver configured
	device_status |= MODULE_VIRTIO_DEVICE_STATUS_driver_ok;
	driver_port_out_byte( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

	//----------------------------------------------------------------------

	// transmit function ready?
	while( ! kernel -> network_tx );

	// hold the door
	while( TRUE ) {
		// properties of frame to send
		uintptr_t frame = EMPTY;

		// acquire data for transmission
		while( ! (frame = kernel -> network_tx()) ) kernel -> time_sleep( TRUE );	// release AP time

		// debug
		// kernel -> log( (uint8_t *) "Tx\n" );

		// resolve properties
		uint8_t *data = (uint8_t *) (frame & STD_PAGE_mask | KERNEL_PAGE_mirror);
		uint64_t length = frame & ~STD_PAGE_mask;

		// move packet content behind header
		if( length + sizeof( struct MODULE_VIRTIO_NETWORK_STRUCTURE_HEADER ) >= STD_PAGE_byte ) {
			// debug
			kernel -> log( (uint8_t *) "[VIRTIO] Tx overflow.\n" );

			// ignore packet
			continue;
		}

		// properties of
		uint16_t index = network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_TX ].driver_address -> index % network -> queue_limit[ MODULE_VIRTIO_NETWORK_QUEUE_TX ];
		struct MODULE_VIRTIO_STRUCTURE_DESCRIPTOR *descriptor	= (struct MODULE_VIRTIO_STRUCTURE_DESCRIPTOR *) &network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_TX ].descriptor_address[ index ];
		struct MODULE_VIRTIO_STRUCTURE_DRIVER *available	= (struct MODULE_VIRTIO_STRUCTURE_DRIVER *) network -> queue[ MODULE_VIRTIO_NETWORK_QUEUE_TX ].driver_address;
		uint16_t *available_ring				= (uint16_t *) ((uintptr_t) available + offsetof( struct MODULE_VIRTIO_STRUCTURE_DRIVER, ring ));

		// copy packet content to descriptor
		uint8_t *source = (uint8_t *) ((uintptr_t) data | KERNEL_PAGE_mirror);
		uint8_t *target = (uint8_t *) (descriptor -> address | KERNEL_PAGE_mirror);
		for( int64_t i = 0; i < length; i++ ) target[ i + sizeof( struct MODULE_VIRTIO_NETWORK_STRUCTURE_HEADER ) ] = source[ i ];

		// set descriptor
		descriptor -> limit = length + sizeof( struct MODULE_VIRTIO_NETWORK_STRUCTURE_HEADER );

		// prepare header
		struct MODULE_VIRTIO_NETWORK_STRUCTURE_HEADER *header = (struct MODULE_VIRTIO_NETWORK_STRUCTURE_HEADER *) target;
		header -> flags		= EMPTY;
		header -> gso_type	= EMPTY;
		header -> header_length	= sizeof( struct MODULE_VIRTIO_NETWORK_STRUCTURE_HEADER );
		header -> gso_size	= EMPTY;
		header -> csum_start	= EMPTY;
		header -> csum_limit	= EMPTY;

		// debug
		// kernel -> log( (uint8_t *) "\n" ); for( uint64_t i = 0; i < (descriptor -> limit % 0x10) + TRUE; i++ ) { kernel -> log( (uint8_t *) "0x%8X", (uintptr_t) target + (i * 0x10) ); for( uint64_t k = i * 0x10; k < ((i * 0x10) + 0x10); k++ ) kernel -> log( (uint8_t *) " %2X", target[ k ] ); kernel -> log( (uint8_t *) "\n" ); }

		// add to available ring
		available_ring[ index ] = index;

		// set next available index
		available -> index++;

		// synchronize memory with host
		MACRO_SYNC();

		// inform about updated available queue
		driver_port_out_word( module_virtio[ network -> id ].base_address + MODULE_VIRTIO_REGISTER_queue_notify, MODULE_VIRTIO_NETWORK_QUEUE_TX );

		// release frame
		kernel -> memory_release_page( (uintptr_t) data );
	}
}
