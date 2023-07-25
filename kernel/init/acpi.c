/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_acpi( void ) {
	// RSDP pointer available?
	if( limine_rsdp_request.response == NULL || ! limine_rsdp_request.response -> address ) {
		// show error
		lib_terminal_printf( &kernel_terminal, "Hello Darkness, My Old Friend." );

		// hold the door
		while( TRUE );
	}

	// RSDP or XSDP header properties
	struct KERNEL_INIT_ACPI_STRUCTURE_RSDP_OR_XSDP_HEADER *local_rsdp_or_xsdp_header = (struct KERNEL_INIT_ACPI_STRUCTURE_RSDP_OR_XSDP_HEADER *) limine_rsdp_request.response -> address;

	// amount of entries
	uint64_t local_list_length = EMPTY;

	// pointer to list depending on revision
	uint32_t *local_list_rsdt_address = EMPTY;
	uint64_t *local_list_xsdt_address = EMPTY;

	// check revision number of RSDP header
	if( local_rsdp_or_xsdp_header -> revision == EMPTY ) {
		// show message regarding ACPI
		lib_terminal_printf( &kernel_terminal, "RSDT (Root System Description Pointer) found.\n" );

		// RSDT header properties
		struct KERNEL_INIT_ACPI_STRUCTURE_DEFAULT *local_rsdt = (struct KERNEL_INIT_ACPI_STRUCTURE_DEFAULT *) ((uintptr_t) local_rsdp_or_xsdp_header -> rsdt_address);
	
		// amount of entries
		local_list_length = (local_rsdt -> length - sizeof( struct KERNEL_INIT_ACPI_STRUCTURE_DEFAULT )) >> STD_SHIFT_4;

		// pointer to list of RSDT entries
		local_list_rsdt_address = (uint32_t *) ((uintptr_t) local_rsdp_or_xsdp_header -> rsdt_address + sizeof( struct KERNEL_INIT_ACPI_STRUCTURE_DEFAULT ));
	} else {
		// show message regarding ACPI
		lib_terminal_printf( &kernel_terminal, "XSDT (eXtended System Descriptor Table) found.\n" );

		// XSDT header properties
		struct KERNEL_INIT_ACPI_STRUCTURE_DEFAULT *local_xsdt = (struct KERNEL_INIT_ACPI_STRUCTURE_DEFAULT *) ((uintptr_t) local_rsdp_or_xsdp_header -> xsdt_address);

		// amount of entries
		local_list_length = (local_xsdt -> length - sizeof( struct KERNEL_INIT_ACPI_STRUCTURE_DEFAULT )) >> STD_SHIFT_8;

		// pointer to list of XSDT entries
		local_list_xsdt_address = (uint64_t *) ((uintptr_t) local_rsdp_or_xsdp_header -> xsdt_address + sizeof( struct KERNEL_INIT_ACPI_STRUCTURE_DEFAULT ));
	}

	// do recon on all entries of list
	for( uint64_t i = 0; i < local_list_length; i++ ) {
		// initialize local variable
		uint64_t local_entry = EMPTY;

		// get address of table from a given entry on list
		if( local_rsdp_or_xsdp_header -> revision == EMPTY )
			// with RSDT in case of ACPI 1.0
			local_entry = local_list_rsdt_address[ i ];
		else
			// or XSDT in case of ACPI 2.0+
			local_entry = local_list_xsdt_address[ i ];

		// if entry contains an MADT signature (Multiple APIC Description Table)
		struct KERNEL_INIT_ACPI_STRUCTURE_MADT *local_madt = (struct KERNEL_INIT_ACPI_STRUCTURE_MADT *) local_entry;
		if( local_madt -> signature == KERNEL_INIT_ACPI_MADT_signature ) {
			// store base address and size of LAPIC entry
			kernel -> lapic_base_address = (struct KERNEL_LAPIC_STRUCTURE *) (uintptr_t) (local_madt -> lapic_address | KERNEL_PAGE_logical);

			// show message regarding LAPIC
			lib_terminal_printf( &kernel_terminal, " LAPIC base address 0x%X\n", (uint64_t) kernel -> lapic_base_address );

			// length of MADT list
			uint64_t local_size = (uint32_t) local_madt -> length - sizeof( struct KERNEL_INIT_ACPI_STRUCTURE_MADT );
		
			// pointer of MADT list
			uint8_t *local_list = (uint8_t *) local_entry + sizeof( struct KERNEL_INIT_ACPI_STRUCTURE_MADT );

			// process all MADT list entries
			while( local_size ) {
				// get size of entry being processed
				struct KERNEL_INIT_ACPI_STRUCTURE_MADT_ENTRY *local_entry = (struct KERNEL_INIT_ACPI_STRUCTURE_MADT_ENTRY *) local_list;
				uint8_t local_entry_length = (uint8_t) local_entry -> length;

				// I/O APIC entry found?
				struct KERNEL_INIT_ACPI_STRUCTURE_IO_APIC *local_io_apic = (struct KERNEL_INIT_ACPI_STRUCTURE_IO_APIC *) local_list;
				if( local_io_apic -> type == KERNEL_INIT_ACPI_APIC_TYPE_io_apic ) {
					// I/O APIC supports interrupt vectors 0+?
					if( local_io_apic -> gsib == EMPTY ) {
						// store base address of I/O APIC
						kernel -> io_apic_base_address = (struct KERNEL_IO_APIC_STRUCTURE_REGISTER *) (uintptr_t) (local_io_apic -> base_address | KERNEL_PAGE_logical);

						// register available IRQ lines
						kernel -> io_apic_irq_lines = -1;	// all available

						// show message regarding I/O APIC
						lib_terminal_printf( &kernel_terminal, " I/O APIC base address 0x%X\n", (uint64_t) kernel -> io_apic_base_address );
					}
				}

				// check next entry on list
				local_list += local_entry_length;
				local_size -= local_entry_length;
			}
		}

		// if entry contains an HPET signature (High-Precision Event Timer)
		struct KERNEL_INIT_ACPI_STRUCTURE_HPET *local_hpet = (struct KERNEL_INIT_ACPI_STRUCTURE_HPET *) local_entry;
		if( local_hpet -> signature == KERNEL_INIT_ACPI_HPET_signature ) {
			// store base address of HPET
			kernel -> hpet_base_address = (struct KERNEL_HPET_STRUCTURE_REGISTER *) (uintptr_t) (local_hpet -> base_address | KERNEL_PAGE_logical);

			// show message regarding HPET
			lib_terminal_printf( &kernel_terminal, " HPET base address 0x%X\n", (uint64_t) kernel -> hpet_base_address );
		}
	}
}
