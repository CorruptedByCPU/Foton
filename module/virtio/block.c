/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void module_virtio_block( void ) {
	// configure only first device
	uint64_t i = 0; for( ; i < module_virtio_limit; i++ ) if( module_virtio[ i ].type == MODULE_VIRTIO_TYPE_block ) break;

	// initialize VirtIO Block properties
	module_virtio[ i ].device = kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_VIRTIO_BLOCK_STRUCTURE ) ) >> STD_SHIFT_PAGE );
	struct MODULE_VIRTIO_BLOCK_STRUCTURE *block = (struct MODULE_VIRTIO_BLOCK_STRUCTURE *) module_virtio[ i ].device;

	// module entry
	block -> id = i;

	// debug
	// kernel -> log( (uint8_t *) "VirtIO-Blk.\n" );

	// properties of device features and status
	uint64_t device_features;
	uint8_t device_status;

	//----------------------------------------------------------------------

	// reset device
	device_status = EMPTY;
	driver_port_out_byte( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

	//----------------------------------------------------------------------

	// device recognized
	device_status |= MODULE_VIRTIO_DEVICE_STATUS_acknowledge;
	driver_port_out_byte( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

	// driver available
	device_status |= MODULE_VIRTIO_DEVICE_STATUS_driver_available;
	driver_port_out_byte( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

	//----------------------------------------------------------------------

	// retrieve device features
	device_features = driver_port_in_dword( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_device_features );

	//----------------------------------------------------------------------

	// inform about supported/required features by driver
	uint32_t quest_features = EMPTY;
	driver_port_out_dword( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_guest_features, device_features );

	// close negotiations
	device_status |= MODULE_VIRTIO_DEVICE_STATUS_features_ok;
	driver_port_out_byte( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

	// retrieve current device status
	device_status = driver_port_in_byte( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_device_status );

	// requested features, accepted?
	if( ! (device_status & MODULE_VIRTIO_DEVICE_STATUS_features_ok) ) return;	// no

	//----------------------------------------------------------------------

	// select queue
	driver_port_out_word( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_queue_select, EMPTY );

	// check if queue exist
	block -> queue_limit = driver_port_in_word( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_queue_limit );
	if( ! block -> queue_limit ) return;	// doesn't

	// rings sizes
	uint64_t limit_cache = MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_VIRTIO_STRUCTURE_DESCRIPTOR ) * block -> queue_limit );
	uint64_t limit_available = MACRO_PAGE_ALIGN_UP( (sizeof( uint16_t ) * block -> queue_limit) + (3 * sizeof( uint16_t )) );
	uint64_t limit_used = MACRO_PAGE_ALIGN_UP( (sizeof( struct MODULE_VIRTIO_STRUCTURE_RING ) * block -> queue_limit) + (3 * sizeof( uint16_t )) );

	// acquire area for queue
	block -> queue.descriptor_address = (struct MODULE_VIRTIO_STRUCTURE_DESCRIPTOR *) (kernel -> memory_alloc_low( (limit_cache + limit_available + limit_used) >> STD_SHIFT_PAGE ) | KERNEL_PAGE_mirror);
	block -> queue.driver_address = (struct MODULE_VIRTIO_STRUCTURE_DRIVER *) ((uintptr_t) block -> queue.descriptor_address + limit_cache);
	block -> queue.device_address = (struct MODULE_VIRTIO_STRUCTURE_DEVICE *) ((uintptr_t) block -> queue.descriptor_address + limit_cache + limit_available);

	// register queue
	driver_port_out_dword( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_queue_address, ((uintptr_t) block -> queue.descriptor_address & ~KERNEL_PAGE_mirror) >> STD_SHIFT_PAGE );

	//----------------------------------------------------------------------

	// // fill Request Queue
	// for( uint64_t i = 0; i < block -> queue_limit; i++ ) {
	// 	// allocate area in Descriptors Queue
	// 	block -> queue.descriptor_address[ i ].address = (uintptr_t) kernel -> memory_alloc_page();
	// 	block -> queue.descriptor_address[ i ].limit = STD_PAGE_byte;
	// 	block -> queue.descriptor_address[ i ].flags = EMPTY;

	// 	// add cache to available ring
	// 	uint16_t *receive_ring_available = (uint16_t *) (block -> queue.driver_address + offsetof( struct MODULE_VIRTIO_STRUCTURE_DRIVER, ring ));
	// 	receive_ring_available[ i ] = i;

	// 	// synchronize memory with host
	// 	MACRO_SYNC();

	// 	// set next available index
	// 	block -> queue.driver_address -> index++;

	// 	// synchronize memory with host
	// 	MACRO_SYNC();
	// }

	// // inform about updated Request Queue
	// driver_port_out_word( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_queue_notify, EMPTY );

	//----------------------------------------------------------------------

	// driver configured
	device_status |= MODULE_VIRTIO_DEVICE_STATUS_driver_ok;
	driver_port_out_byte( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

	//----------------------------------------------------------------------

	kernel -> log( (uint8_t *) "OK.\n" );








		// properties of
		uint16_t index 						= block -> queue.driver_address -> index % block -> queue_limit;
		struct MODULE_VIRTIO_STRUCTURE_DESCRIPTOR *descriptor	= (struct MODULE_VIRTIO_STRUCTURE_DESCRIPTOR *) &block -> queue.descriptor_address[ index ];
		struct MODULE_VIRTIO_STRUCTURE_DRIVER *driver		= (struct MODULE_VIRTIO_STRUCTURE_DRIVER *) block -> queue.driver_address;
		uint16_t *driver_ring					= (uint16_t *) ((uintptr_t) driver + offsetof( struct MODULE_VIRTIO_STRUCTURE_DRIVER, ring ));

		// set descriptor
		descriptor -> address = (uintptr_t) kernel -> memory_alloc_page();
		descriptor -> limit = sizeof( struct MODULE_VIRTIO_BLOCK_STRUCTURE_REQUEST );
		descriptor -> flags = MODULE_VIRTIO_DESCRIPTOR_FLAG_WRITE;

		// set request properties
		struct MODULE_VIRTIO_BLOCK_STRUCTURE_REQUEST *request = (struct MODULE_VIRTIO_BLOCK_STRUCTURE_REQUEST *) (descriptor -> address | KERNEL_PAGE_mirror);
		request -> type		= MODULE_VIRTIO_BLOCK_REQUEST_TYPE_in;
		request -> block	= EMPTY;

		// add to driver ring
		driver_ring[ index ] = index;

		// set next driver index
		driver -> index++;

		// synchronize memory with host
		MACRO_SYNC();

		// inform about updated driver queue
		driver_port_out_word( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_queue_notify, EMPTY );

		while( ! request -> status ) MACRO_DEBUF();

		kernel -> log( (uint8_t *) "%b\n", request -> status );











	// hodor
	while( TRUE ) kernel -> time_sleep( TRUE );	// release AP time
}

void module_virtio_block_request_read( uint64_t id, uint64_t sector, uint8_t *data, uint64_t length ) {
	// properties of request
	struct MODULE_VIRTIO_BLOCK_STRUCTURE_REQUEST *request = (struct MODULE_VIRTIO_BLOCK_STRUCTURE_REQUEST *) ((uintptr_t) kernel -> memory_alloc_page() | KERNEL_PAGE_mirror);

	// read data from device
	request -> type = MODULE_VIRTIO_BLOCK_REQUEST_TYPE_in;
	request -> block = EMPTY;	// first one

	// properties of device
	struct MODULE_VIRTIO_BLOCK_STRUCTURE *block = (struct MODULE_VIRTIO_BLOCK_STRUCTURE *) module_virtio[ id ].device;
	

	// available descriptor id
	uint64_t i = block -> queue.descriptor_index;

	// set first descriptor
	block -> queue.descriptor_address[ i ].address = (uintptr_t) request & ~KERNEL_PAGE_mirror;
	block -> queue.descriptor_address[ i ].limit = offsetof( struct MODULE_VIRTIO_BLOCK_STRUCTURE_REQUEST, data );
	block -> queue.descriptor_address[ i ].flags = MODULE_VIRTIO_DESCRIPTOR_FLAG_NEXT;
	block -> queue.descriptor_address[ i ].next = i + 1; i++;

	// second descriptor
	block -> queue.descriptor_address[ i ].address = ((uintptr_t) request & ~KERNEL_PAGE_mirror) + offsetof( struct MODULE_VIRTIO_BLOCK_STRUCTURE_REQUEST, data );
	block -> queue.descriptor_address[ i ].limit = 512;	// Bytes
	block -> queue.descriptor_address[ i ].flags = MODULE_VIRTIO_DESCRIPTOR_FLAG_NEXT | MODULE_VIRTIO_DESCRIPTOR_FLAG_WRITE;
	block -> queue.descriptor_address[ i ].next = i + 1; i++;

	// third descriptor
	block -> queue.descriptor_address[ i ].address = ((uintptr_t) request & ~KERNEL_PAGE_mirror) + offsetof( struct MODULE_VIRTIO_BLOCK_STRUCTURE_REQUEST, status );
	block -> queue.descriptor_address[ i ].limit = MACRO_SIZEOF( struct MODULE_VIRTIO_BLOCK_STRUCTURE_REQUEST, status );
	block -> queue.descriptor_address[ i ].flags = MODULE_VIRTIO_DESCRIPTOR_FLAG_WRITE;
}

void module_virtio_block_request_write( uint64_t id, uint64_t block, uint8_t *data, uint64_t length ) {
	
}