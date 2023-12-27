/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_HPET
	#define KERNEL_HPET

	#define	KERNEL_HPET_TIMER_offset	0x0100

	struct KERNEL_HPET_STRUCTURE_REGISTER {
		uint64_t	general_capabilities_and_id;
		uint64_t	reserved_0;
		uint64_t	general_configuration;
		uint64_t	reserved_1;
		uint64_t	general_interrupt_status;
		uint64_t	reserved_2[ 25 ];
		uint64_t	main_counter_value;
		uint64_t	reserved_3;
	} __attribute__( (packed) );

	struct KERNEL_HPET_STRUCTURE_TIMER {
		uint64_t	configuration_and_capabilities;
		uint64_t	comparator;
		uint64_t	fsb_interrupt_route;
		uint64_t	reserved;
	} __attribute__( (packed) );

	// external routine (assembly language)
	extern void kernel_hpet_uptime_entry( void );

	// function counting miliseconds
	__attribute__(( no_caller_saved_registers ))
	void kernel_hpet_uptime( void );
#endif
