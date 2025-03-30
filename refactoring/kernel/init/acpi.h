/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_INIT_ACPI
	#define	KERNEL_INIT_ACPI

	#define	KERNEL_INIT_ACPI_MADT_signature		0x43495041

	#define	KERNEL_INIT_ACPI_APIC_TYPE_lapic	0
	#define	KERNEL_INIT_ACPI_APIC_TYPE_io_apic	1

	struct	KERNEL_STRUCTURE_INIT_ACPI_RSDP_OR_XSDP_HEADER {
		uint64_t	signature;
		uint8_t		checksum;
		uint8_t		oem_id[ 6 ];
		uint8_t		revision;
		uint32_t	rsdt_address;
		uint32_t	length;
		uint64_t	xsdt_address;
		uint8_t		extended_checksum;
		uint8_t		reserved[ 3 ];
	} __attribute__( (packed) );

	struct	KERNEL_STRUCTURE_INIT_ACPI_DEFAULT {
		uint32_t	signature;
		uint32_t	length;
		uint8_t		revision;
		uint8_t		checksum;
		uint8_t		oem_id[ 6 ];
		uint64_t	oem_table_id;
		uint32_t	oem_revision;
		uint32_t	creator_id;
		uint32_t	creator_revision;
	} __attribute__( (packed) );

	struct	KERNEL_STRUCTURE_INIT_ACPI_MADT {
		uint32_t	signature;
		uint32_t	length;
		uint8_t		revision;
		uint8_t		checksum;
		uint8_t		oem_id[ 6 ];
		uint64_t	oem_table_id;
		uint32_t	oem_revision;
		uint32_t	creator_id;
		uint32_t	creator_revision;
		//-------------------------------
		uint32_t	lapic_address;
		uint32_t	flags;
	} __attribute__( (packed) );

	struct	KERNEL_STRUCTURE_INIT_ACPI_MADT_ENTRY {
		uint8_t		type;
		uint8_t		length;
	} __attribute__( (packed) );

	struct	KERNEL_STRUCTURE_INIT_ACPI_APIC {
		uint8_t		type;
		uint8_t		length;
	} __attribute__( (packed) );

	struct	KERNEL_STRUCTURE_INIT_ACPI_IO_APIC {
		uint8_t		type;
		uint8_t		length;
		uint8_t		ioapic_id;
		uint8_t		reserved;
		uint32_t	base_address;
		uint32_t	gsib;	// Global System Interrupt Base
	} __attribute__( (packed) );
#endif
