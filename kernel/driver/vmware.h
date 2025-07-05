/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of driver
	//----------------------------------------------------------------------
	#ifndef	DRIVER_SERIAL
		#include	"./vmware.h"
	#endif

struct DRIVER_VMWARE_STRUCTURE_REQUEST {
	uint32_t	magic;
	uint32_t	size;
	uint32_t	command;
	uint32_t	port;
};

struct DRIVER_VMWARE_STRUCTURE_MOUSE {
	uint32_t	status;
	uint32_t	x;
	uint32_t	y;
	uint32_t	z;
};

#define DRIVER_VMWARE_MAGIC			0x564D5868
#define DRIVER_VMWARE_PORT			0x5658

#define	DRIVER_VMWARE_COMMAND_GET_VERSION	10
#define DRIVER_VMWARE_COMMAND_POINTER_DATA	39
#define DRIVER_VMWARE_COMMAND_POINTER_STATUS	40
#define DRIVER_VMWARE_COMMAND_POINTER		41

#define DRIVER_VMWARE_POINTER_ENABLE   		0x45414552
#define DRIVER_VMWARE_POINTER_RELATIVE 		0xF5
#define DRIVER_VMWARE_POINTER_ABSOLUTE 		0x53424152
