/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/string.h"

uint16_t lib_network_checksum( uint16_t *data, uint16_t length );

// converts string of IPV4 address "16.32.48.64" as DWORD 0x10203040
uint32_t lib_network_string_to_ipv4( uint8_t *string );
