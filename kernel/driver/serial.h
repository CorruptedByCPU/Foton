/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	DRIVER_SERIAL
	#define	DRIVER_SERIAL

	#define	DRIVER_SERIAL_PORT_COM1					0x03F8

	#define	DRIVER_SERIAL_REGISTER_data_or_divisor_low		0x00
	#define	DRIVER_SERIAL_REGISTER_interrupt_enable_or_divisor_high	0x01
	#define	DRIVER_SERIAL_REGISTER_interrupt_identification_or_fifo	0x02
	#define	DRIVER_SERIAL_REGISTER_line_control_or_dlab		0x03
	#define	DRIVER_SERIAL_REGISTER_modem_control			0x04
	#define	DRIVER_SERIAL_REGISTER_line_status			0x05
	#define	DRIVER_SERIAL_REGISTER_modem_status			0x06
	#define	DRIVER_SERIAL_REGISTER_scratch				0x07

	void driver_serial_init( void );
	void driver_serial_char( uint8_t byte );
	void driver_serial_memory( uintptr_t address, int64_t length );
	void driver_serial_pool_in( void );
	void driver_serial_pool_out( void );
	void driver_serial_string( uint8_t *string, uint64_t length );
	uint8_t driver_serial_value( uint64_t value, uint8_t base, uint8_t prefix, uint8_t character );
#endif
