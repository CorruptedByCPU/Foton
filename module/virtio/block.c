/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void module_virtio_block( void ) {
	// configure only first device
	uint64_t i = 0; for( ; i < module_virtio_limit; i++ ) if( module_virtio[ i ].type == MODULE_VIRTIO_TYPE_block ) break;

	// initialize VirtIO Network properties
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

	// kernel -> log( (uint8_t *) "0x%X\n", driver_port_in_qword( module_virtio[ block -> id ].base_address + MODULE_VIRTIO_REGISTER_device_config + offsetof( struct MODULE_VIRTIO_BLOCK_STRUCTURE_DEVICE_CONFIG, capacity ) ) );	

	// hodor
	while( TRUE ) kernel -> time_sleep( TRUE );	// release AP time
}
