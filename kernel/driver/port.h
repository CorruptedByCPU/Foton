/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef DRIVER_PORT
	#define	DRIVER_PORT

	// send 8 bit value to selected port
	inline void driver_port_out_byte( uint16_t port, uint8_t value );

	// send 32 bit value to selected port
	inline void driver_port_out_dword( uint16_t port, uint32_t value );

	// get 8 bit value from selected port
	inline uint8_t driver_port_in_byte( uint16_t port );
	
	// get 32 bit value from selected port
	inline uint32_t driver_port_in_dword( uint16_t port );

	// get 64 bit value from selected port
	inline uint64_t driver_port_in_qword( uint16_t port );
#endif
