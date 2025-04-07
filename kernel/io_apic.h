/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_IOAPIC
	#define KERNEL_IOAPIC

	#define	KERNEL_IO_APIC_ioregsel			0x00
	#define	KERNEL_IO_APIC_iowin			0x10
	#define	KERNEL_IO_APIC_iowin_low		0x00
	#define	KERNEL_IO_APIC_iowin_high		0x01

	#define	KERNEL_IO_APIC_TRIGER_MODE_level	1000000000000000b

	struct KERNEL_STRUCTURE_IO_APIC_REGISTER {
		uint32_t	ioregsel;
		uint32_t	reserved[ 3 ];
		uint32_t	iowin;
	} __attribute__( (packed) );

	// check if irq line is available for use
	uint8_t kernel_io_apic_line( uint8_t irq );

	// connect I/O APIC interrupt line to IDT
	void kernel_io_apic_connect( uint8_t irq, uint32_t io_apic_register );
#endif
