/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_acpi( void ) {
	// RSDP or XSDP header properties
	struct KERNEL_STRUCTURE_INIT_ACPI_RSDP_OR_XSDP_HEADER *rsdp_or_xsdp_header = (struct KERNEL_STRUCTURE_INIT_ACPI_RSDP_OR_XSDP_HEADER *) limine_rsdp_request.response -> address;

	// amount of entries
	uint64_t list_length = EMPTY;

	// pointer to list depending on revision
	uint32_t *rsdt_address = EMPTY;
	uint64_t *xsdt_address = EMPTY;

	// check revision number of header
	if( rsdp_or_xsdp_header -> revision < 0x02 ) {
		// RSDT header properties
		struct KERNEL_STRUCTURE_INIT_ACPI_DEFAULT *rsdt = (struct KERNEL_STRUCTURE_INIT_ACPI_DEFAULT *) ((uintptr_t) rsdp_or_xsdp_header -> rsdt_address);
	
		// amount of entries
		list_length = (rsdt -> length - sizeof( struct KERNEL_STRUCTURE_INIT_ACPI_DEFAULT )) >> STD_SHIFT_4;

		// pointer to list of RSDT entries
		rsdt_address = (uint32_t *) ((uintptr_t) rsdp_or_xsdp_header -> rsdt_address + sizeof( struct KERNEL_STRUCTURE_INIT_ACPI_DEFAULT ));
	} else {
		// XSDT header properties
		struct KERNEL_STRUCTURE_INIT_ACPI_DEFAULT *xsdt = (struct KERNEL_STRUCTURE_INIT_ACPI_DEFAULT *) ((uintptr_t) rsdp_or_xsdp_header -> xsdt_address);

		// amount of entries
		list_length = (xsdt -> length - sizeof( struct KERNEL_STRUCTURE_INIT_ACPI_DEFAULT )) >> STD_SHIFT_8;

		// pointer to list of XSDT entries
		xsdt_address = (uint64_t *) ((uintptr_t) rsdp_or_xsdp_header -> xsdt_address + sizeof( struct KERNEL_STRUCTURE_INIT_ACPI_DEFAULT ));
	}

	// do recon on all entries of list
	for( uint64_t i = 0; i < list_length; i++ ) {
		// SDT pointer address
		uintptr_t sdt = EMPTY;

		// get address of table from a given entry on list
		if( rsdp_or_xsdp_header -> revision < 0x02 )
			// with RSDT in case of ACPI 1.0
			sdt = rsdt_address[ i ];
		else
			// or XSDT in case of ACPI 2.0+
			sdt = xsdt_address[ i ];

		// if entry contains an MADT signature (Multiple APIC Description Table)
		struct KERNEL_STRUCTURE_INIT_ACPI_MADT *madt = (struct KERNEL_STRUCTURE_INIT_ACPI_MADT *) sdt;
		if( madt -> signature == KERNEL_INIT_ACPI_MADT_signature ) {
			// store APIC base address
			kernel -> apic_base_address = (struct KERNEL_STRUCTURE_APIC *) (uintptr_t) (madt -> lapic_address | KERNEL_MEMORY_mirror);

			// length of MADT list
			uint64_t limit = (uint32_t) madt -> length - sizeof( struct KERNEL_STRUCTURE_INIT_ACPI_MADT );
		
			// pointer of MADT list
			uint8_t *list = (uint8_t *) sdt + sizeof( struct KERNEL_STRUCTURE_INIT_ACPI_MADT );

			// process all MADT list entries
			while( limit ) {
				// get size of entry being processed
				struct KERNEL_STRUCTURE_INIT_ACPI_MADT_ENTRY *sdt = (struct KERNEL_STRUCTURE_INIT_ACPI_MADT_ENTRY *) list;
				uint8_t sdt_length = (uint8_t) sdt -> length;

				// I/O APIC entry found?
				struct KERNEL_STRUCTURE_INIT_ACPI_IO_APIC *io_apic = (struct KERNEL_STRUCTURE_INIT_ACPI_IO_APIC *) list;
				if( io_apic -> type == KERNEL_INIT_ACPI_APIC_TYPE_io_apic ) {
					// I/O APIC supports interrupt vectors 0+?
					if( io_apic -> gsib == EMPTY ) {
						// store base address of I/O APIC
						kernel -> io_apic_base_address = (struct KERNEL_STRUCTURE_IO_APIC *) (uintptr_t) (io_apic -> base_address | KERNEL_MEMORY_mirror);
					}
				}

				// check next entry on list
				list += sdt_length;
				limit -= sdt_length;
			}
		}
	}
}
