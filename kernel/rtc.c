/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of driver
	//----------------------------------------------------------------------
	#ifndef	KERNEL_RTC
		#include	"./rtc.h"
	#endif

__attribute__(( no_caller_saved_registers ))
void kernel_rtc() {
	// receive pending interrupt of the real-time controller
	driver_port_out_byte( KERNEL_RTC_PORT_command, KERNEL_RTC_STATUS_REGISTER_C );
	driver_port_in_byte( KERNEL_RTC_PORT_data );

	// increase the real-time controller invocation count
	kernel -> time_unit++;

	// accept current interrupt call
	kernel_lapic_accept();
}