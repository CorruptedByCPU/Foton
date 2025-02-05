/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void module_virtio_block( void ) {
	// initialize VirtIO Block properties
	module_virtio_blk = (struct MODULE_VIRTIO_BLK_STRUCTURE *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_VIRTIO_BLK_STRUCTURE ) * TRUE ) >> STD_SHIFT_PAGE );

	// debug
	// kernel -> log( (uint8_t *) "VirtIO-Blk.\n" );

	// properties of device features and status
	uint64_t device_features;
	uint8_t device_status;

	//----------------------------------------------

	// reset device
	device_status = EMPTY;
	driver_port_out_byte( module_virtio[ module_virtio_network_id ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

	//----------------------------------------------

	// device recognized
	device_status |= MODULE_VIRTIO_DEVICE_STATUS_acknowledge;
	driver_port_out_byte( module_virtio[ module_virtio_network_id ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

	// driver available
	device_status |= MODULE_VIRTIO_DEVICE_STATUS_driver_available;
	driver_port_out_byte( module_virtio[ module_virtio_network_id ].base_address + MODULE_VIRTIO_REGISTER_device_status, device_status );

	//----------------------------------------------

	// retrieve device features
	device_features = driver_port_in_dword( module_virtio[ module_virtio_network_id ].base_address + MODULE_VIRTIO_REGISTER_device_features );

	// hodor
	while( TRUE ) kernel -> time_sleep( TRUE );	// release AP time
}
