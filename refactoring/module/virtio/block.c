/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void module_virtio_block( void ) {
	// configure only first device
	for( uint64_t i = 0; i < module_virtio_limit; i++ ) {
		// not a block device?
		if( module_virtio[ i ].type != MODULE_VIRTIO_TYPE_block ) continue;	// ignore

		// initialize VirtIO Block properties
		module_virtio[ i ].device = kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_VIRTIO_BLOCK_STRUCTURE ) ) >> STD_SHIFT_PAGE );
		struct MODULE_VIRTIO_BLOCK_STRUCTURE *block = (struct MODULE_VIRTIO_BLOCK_STRUCTURE *) module_virtio[ i ].device;

		// module entry
		block -> id = i;

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
		if( ! (device_status & MODULE_VIRTIO_DEVICE_STATUS_features_ok) ) { kernel -> log( (uint8_t *) "[Virt I/O] Block #%u, features not accepted.\n", block -> id ); continue; }	// no

		//----------------------------------------------------------------------

		// select queue
		driver_port_out_word( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_queue_select, EMPTY );

		// check if queue exist
		block -> queue_limit = driver_port_in_word( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_queue_limit );
		if( ! block -> queue_limit ) { kernel -> log( (uint8_t *) "[Virt I/O] Block #%u, no queue.\n", block -> id ); continue; }	// doesn't

		// rings sizes
		uint64_t limit_cache = MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_VIRTIO_STRUCTURE_DESCRIPTOR ) * block -> queue_limit );
		uint64_t limit_available = MACRO_PAGE_ALIGN_UP( (sizeof( uint16_t ) * block -> queue_limit) + (3 * sizeof( uint16_t )) );
		uint64_t limit_used = MACRO_PAGE_ALIGN_UP( (sizeof( struct MODULE_VIRTIO_STRUCTURE_RING ) * block -> queue_limit) + (3 * sizeof( uint16_t )) );

		// acquire area for queue
		block -> queue.descriptor_address = (struct MODULE_VIRTIO_STRUCTURE_DESCRIPTOR *) (kernel -> memory_alloc_low( MACRO_PAGE_ALIGN_UP( limit_cache + limit_available + limit_used ) >> STD_SHIFT_PAGE ) | KERNEL_MEMORY_mirror);
		block -> queue.driver_address = (struct MODULE_VIRTIO_STRUCTURE_DRIVER *) ((uintptr_t) block -> queue.descriptor_address + limit_cache);
		block -> queue.device_address = (struct MODULE_VIRTIO_STRUCTURE_DEVICE *) ((uintptr_t) block -> queue.descriptor_address + limit_cache + limit_available);

		// register queue
		driver_port_out_dword( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_queue_address, ((uintptr_t) block -> queue.descriptor_address & ~KERNEL_MEMORY_mirror) >> STD_SHIFT_PAGE );

		//----------------------------------------------------------------------

		// driver configured
		device_status |= MODULE_VIRTIO_DEVICE_STATUS_driver_ok;
		driver_port_out_byte( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

		//----------------------------------------------------------------------

		MACRO_DEBUF();

		// register as storage
		// struct KERNEL_STRUCTURE_STORAGE *storage = kernel -> storage_add();

		// storage class
		// storage -> type = STD_STORAGE_TYPE_disk;

		// // set storage name
		// storage -> length = 2;
		// uint8_t string_name[] = "vd"; for( uint8_t k = 0; k < storage -> length; k++ ) storage -> name[ k ] = string_name[ k ];
		// storage -> length += lib_integer_to_string( block -> id, STD_NUMBER_SYSTEM_decimal, (uint8_t *) &storage -> name[ storage -> length ] );

		// address of main block location
		// storage -> id = block -> id;

		// first usable device block
		// storage -> block = EMPTY;

		// default block size in Bytes
		// storage -> byte = 512;

		// length of storage in Blocks
		// storage -> limit = driver_port_in_qword( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_BLOCK_STRUCTURE_DEVICE_CONFIG, capacity ) ) * storage -> byte;

		// attach read/write functions
		// storage -> block_read = (void *) module_virtio_block_request_read;
		// storage -> block_write = (void *) module_virtio_block_request_write;

		// storage active
		// storage -> flags |= KERNEL_STORAGE_FLAGS_active;
	}

	//----------------------------------------------------------------------

	// hodor
	while( TRUE ) kernel -> time_sleep( TRUE );	// release AP time
}

// void module_virtio_block_transfer( uint8_t type, uint64_t id, uint64_t sector, uint8_t *data, uint64_t length ) {
// 	// properties of request
// 	struct MODULE_VIRTIO_BLOCK_STRUCTURE_REQUEST *request = (struct MODULE_VIRTIO_BLOCK_STRUCTURE_REQUEST *) ((uintptr_t) kernel -> memory_alloc_page() | KERNEL_MEMORY_mirror);

// 	// read data from device
// 	request -> type = type;
// 	request -> block = sector;

// 	// read or write?
// 	uint16_t flag = MODULE_VIRTIO_DESCRIPTOR_FLAG_READ;	// by default
// 	if( type == MODULE_VIRTIO_BLOCK_REQUEST_TYPE_in ) flag = MODULE_VIRTIO_DESCRIPTOR_FLAG_WRITE;

// 	while( length-- ) {
// 		// properties of device
// 		struct MODULE_VIRTIO_BLOCK_STRUCTURE *block = (struct MODULE_VIRTIO_BLOCK_STRUCTURE *) module_virtio[ id ].device;

// 		// set first descriptor
// 		block -> queue.descriptor_address[ 0 ].address = (uintptr_t) request & ~KERNEL_MEMORY_mirror;
// 		block -> queue.descriptor_address[ 0 ].limit = offsetof( struct MODULE_VIRTIO_BLOCK_STRUCTURE_REQUEST, data );
// 		block -> queue.descriptor_address[ 0 ].flags = MODULE_VIRTIO_DESCRIPTOR_FLAG_NEXT;
// 		block -> queue.descriptor_address[ 0 ].next = 1;

// 		// second descriptor
// 		block -> queue.descriptor_address[ 1 ].address = ((uintptr_t) data & ~KERNEL_MEMORY_mirror);
// 		block -> queue.descriptor_address[ 1 ].limit = 512;	// Bytes
// 		block -> queue.descriptor_address[ 1 ].flags = flag | MODULE_VIRTIO_DESCRIPTOR_FLAG_NEXT;
// 		block -> queue.descriptor_address[ 1 ].next = 2;

// 		// third descriptor
// 		block -> queue.descriptor_address[ 2 ].address = ((uintptr_t) request & ~KERNEL_MEMORY_mirror) + offsetof( struct MODULE_VIRTIO_BLOCK_STRUCTURE_REQUEST, status );
// 		block -> queue.descriptor_address[ 2 ].limit = MACRO_SIZEOF( struct MODULE_VIRTIO_BLOCK_STRUCTURE_REQUEST, status );
// 		block -> queue.descriptor_address[ 2 ].flags = MODULE_VIRTIO_DESCRIPTOR_FLAG_WRITE;

// 		struct MODULE_VIRTIO_STRUCTURE_DRIVER *block_driver	= (struct MODULE_VIRTIO_STRUCTURE_DRIVER *) block -> queue.driver_address;
// 		uint16_t *block_driver_ring				= (uint16_t *) ((uintptr_t) block_driver + offsetof( struct MODULE_VIRTIO_STRUCTURE_DRIVER, ring ));

// 		// add to available ring
// 		block_driver_ring[ block_driver -> index % block -> queue_limit ] = 0;

// 		// synchronize memory with host
// 		MACRO_SYNC();

// 		// set next available index
// 		block_driver -> index++;

// 		// synchronize memory with host
// 		MACRO_SYNC();

// 		// inform about updated available queue
// 		driver_port_out_word( module_virtio[ id ].base_address + MODULE_VIRTIO_REGISTER_queue_notify, EMPTY );

// 		// wait for block of data to be transferred
// 		while( ! block -> semaphore ) kernel -> time_sleep( TRUE );

// 		// put down semaphore
// 		block -> semaphore = FALSE;

// 		// next part of data
// 		data += 512;

// 		// inside next sector
// 		request -> block++;
// 	}

// 	// release request area
// 	kernel -> memory_release( (uintptr_t) request, TRUE );
// }

// void module_virtio_block_request_read( uint64_t id, uint64_t block, uint8_t *data, uint64_t length ) {
// 	// read blocks
// 	module_virtio_block_transfer( MODULE_VIRTIO_BLOCK_REQUEST_TYPE_in, id, block, data, length );
// }

// void module_virtio_block_request_write( uint64_t id, uint64_t block, uint8_t *data, uint64_t length ) {
// 	// write blocks
// 	module_virtio_block_transfer( MODULE_VIRTIO_BLOCK_REQUEST_TYPE_out, id, block, data, length );
// }
