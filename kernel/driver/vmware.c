/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of driver
	//----------------------------------------------------------------------
	#ifndef	DRIVER_SERIAL
		#include	"./vmware.h"
	#endif

void driver_vmware_send( struct DRIVER_VMWARE_STRUCTURE_REQUEST *packet ) {
	// set magic value and destined port
	packet -> magic = DRIVER_VMWARE_MAGIC;
	packet -> port = DRIVER_VMWARE_PORT;
	__asm__ volatile( "in %%dx, %0" : "+a" (packet -> magic), "+b" ( packet -> size), "+c" (packet -> command), "+d" (packet -> port) );
}

inline uint16_t driver_vmware_convert( uint16_t value, uint16_t limit ) { return (value * limit) / (uint16_t) STD_MAX_unsigned; }

void driver_vmware_pointer_enable( void ) {
	// properties of VMware packet
	struct DRIVER_VMWARE_STRUCTURE_REQUEST packet = { EMPTY };

	// enable absolute pointer
	packet.size	= DRIVER_VMWARE_POINTER_ENABLE;
	packet.command	= DRIVER_VMWARE_COMMAND_POINTER;
	driver_vmware_send( (struct DRIVER_VMWARE_STRUCTURE_REQUEST *) &packet );

	// receive status of absolute pointer
	packet.size	= TRUE;	// only status field
	packet.command	= DRIVER_VMWARE_COMMAND_POINTER_DATA;
	driver_vmware_send( (struct DRIVER_VMWARE_STRUCTURE_REQUEST *) &packet );

	// enable absolute pointer
	packet.size	= DRIVER_VMWARE_POINTER_ABSOLUTE;
	packet.command	= DRIVER_VMWARE_COMMAND_POINTER;
	driver_vmware_send( (struct DRIVER_VMWARE_STRUCTURE_REQUEST *) &packet );
}

void driver_vmware_pointer_disable( void ) {
	// properties of VMware packet
	struct DRIVER_VMWARE_STRUCTURE_REQUEST packet = { EMPTY };

	// disable pointer
	packet.size	= DRIVER_VMWARE_POINTER_RELATIVE;
	packet.command	= DRIVER_VMWARE_COMMAND_POINTER;
	driver_vmware_send( (struct DRIVER_VMWARE_STRUCTURE_REQUEST *) &packet );
}

void drvier_vmware_mouse( void ) {
	// properties of VMware packet
	struct DRIVER_VMWARE_STRUCTURE_REQUEST packet = { EMPTY };

	// retrieve device state
	packet.size = EMPTY;
	packet.command = DRIVER_VMWARE_COMMAND_POINTER_STATUS;
	driver_vmware_send( (struct DRIVER_VMWARE_STRUCTURE_REQUEST *) &packet );
	
	// malformed?
	if( packet.magic == 0xFFFF0000 ) {
		// disable absolute pointer
		driver_vmware_pointer_disable();

		// and enable again
		driver_vmware_pointer_enable();

		// done
		return;
	}
	
	// invalid length of data to receive?
	if( (packet.magic & 0xFFFF) < STD_SHIFT_4 ) return;

	// read data
	packet.size	= 4;	// status, x, y, z
	packet.command	= DRIVER_VMWARE_COMMAND_POINTER_DATA;
	driver_vmware_send( (struct DRIVER_VMWARE_STRUCTURE_REQUEST *) &packet );

	// properties of mouse data
	struct DRIVER_VMWARE_STRUCTURE_MOUSE *mouse = (struct DRIVER_VMWARE_STRUCTURE_MOUSE *) &packet;

	// update properties of mouse device

	// buttons
	uint16_t buttons = (mouse -> status & 0xFFFF0000) >> STD_SHIFT_16;
	if( buttons & 0x20 ) kernel -> device_mouse_status |= STD_MOUSE_BUTTON_left; else kernel -> device_mouse_status &= ~STD_MOUSE_BUTTON_left;
	if( buttons & 0x10 ) kernel -> device_mouse_status |= STD_MOUSE_BUTTON_right; else kernel -> device_mouse_status &= ~STD_MOUSE_BUTTON_right;
	if( buttons & 0x08 ) kernel -> device_mouse_status |= STD_MOUSE_BUTTON_middle; else kernel -> device_mouse_status &= ~STD_MOUSE_BUTTON_middle;

	// coordinates
	kernel -> device_mouse_x = driver_vmware_convert( mouse -> x, kernel -> framebuffer_width_pixel );
	kernel -> device_mouse_y = driver_vmware_convert( mouse -> y, kernel -> framebuffer_height_pixel );
	kernel -> device_mouse_z += (int8_t) mouse -> z;
}

void driver_vmware_init( void ) {
	// properties of VMware packet
	struct DRIVER_VMWARE_STRUCTURE_REQUEST packet = { EMPTY };

	// send discovery packet
	packet.size	= ~DRIVER_VMWARE_MAGIC;
	packet.command	= DRIVER_VMWARE_COMMAND_GET_VERSION;
	driver_vmware_send( (struct DRIVER_VMWARE_STRUCTURE_REQUEST *) &packet );

        // VMware tools available?
	if( packet.magic == 0xFFFFFFFF || packet.size != DRIVER_VMWARE_MAGIC ) return;    // no

	// enable absolute pointer
	driver_vmware_pointer_enable();

	// inform any driver about VMware available
	kernel -> device_mouse = drvier_vmware_mouse;
}
