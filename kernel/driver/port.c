/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of driver
	//----------------------------------------------------------------------
	#ifndef	DRIVER_PORT
		#include	"./port.h"
	#endif
	#ifndef	DRIVER_PCI
		#include	"./pci.h"
	#endif

inline void driver_port_out_byte( uint16_t port, uint8_t value ) {
	// send 8 bit value to selected port
	__asm__ volatile( "outb %0, %1" :: "a"( value ), "Nd"( port ) );
}

inline void driver_port_out_word( uint16_t port, uint16_t value ) {
	// send 16 bit value to selected port
	__asm__ volatile( "outw %0, %1" :: "a"( value ), "Nd"( port ) );
}

inline void driver_port_out_dword( uint16_t port, uint32_t value ) {
	// send 32 bit value to selected port
	__asm__ volatile( "outl %0, %1" :: "a"( value ), "Nd"( port ) );
}

inline uint8_t driver_port_in_byte( uint16_t port ) {
	// get 8 bit value from selected port
	uint8_t result;
	__asm__ volatile( "inb %1, %0" : "=a"( result ) : "Nd"( port ) );

	// return that value
	return result;
}

inline uint16_t driver_port_in_word( uint16_t port ) {
	// get 16 bit value from selected port
	uint16_t result;
	__asm__ volatile( "inw %1, %0" : "=a"( result ) : "Nd"( port ) );

	// return that value
	return result;
}

inline uint32_t driver_port_in_dword( uint16_t port ) {
	// get 32 bit value from selected port
	uint32_t result;
	__asm__ volatile( "inl %1, %0" : "=a"( result ) : "Nd"( port ) );

	// return that value
	return result;
}

inline uint64_t driver_port_in_qword( uint16_t port ) {
	// return 64 bit value from selected port
	return (uint64_t) driver_port_in_dword( port ) | ((uint64_t) driver_port_in_dword( port + STD_SIZE_DWORD_byte ) << STD_MOVE_DWORD);
}
