/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of standard library
	//----------------------------------------------------------------------
	#include	"../library/std.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel
	//----------------------------------------------------------------------
	#include	"../kernel/config.h"
	#include	"../kernel/idt.h"
	#include	"../kernel/io_apic.h"
	#include	"../kernel/lapic.h"
	#include	"../kernel/page.h"
	//----------------------------------------------------------------------
	// drivers
	//----------------------------------------------------------------------
	#include	"../kernel/driver/port.h"
	#include	"../kernel/driver/port.c"
	#include	"../kernel/driver/pci.h"
	#include	"../kernel/driver/pci.c"
	//----------------------------------------------------------------------
	// variables, structures, definitions of module
	//----------------------------------------------------------------------
	#include	"./e1000/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./e1000/data.c"

uint32_t driver_e1000_eeprom( uint32_t reg ) {
	// ask about register content
	driver_e1000_mmio_base_address -> eerd = reg;

	// return its value
	return driver_e1000_mmio_base_address -> eerd;
}

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// find device of type "network"
	struct DRIVER_PCI_STRUCTURE pci = driver_pci_find_class_and_subclass( DRIVER_PCI_CLASS_SUBCLASS_network );

	// if found, check model and vendor
	if( driver_pci_read( pci, DRIVER_PCI_REGISTER_vendor_and_device ) == DRIVER_E1000_VENDOR_AND_DEVICE ) {
		// debug
		kernel -> log( (uint8_t *) "[E1000] PCI %2X:%2X.%u - Network Controller found.\n", pci.bus, pci.device, pci.function );

		// get base address of network controller mmio
		uint64_t mmio = (uint64_t) driver_pci_read( pci, DRIVER_PCI_REGISTER_bar0 );

		// base address can be 64 bit wide?
		if( (uint8_t) mmio & DRIVER_PCI_REGISTER_FLAG_64 )
			// get older 32 bit part of full address
			mmio |= (uint64_t) driver_pci_read( pci, DRIVER_PCI_REGISTER_bar1 ) << STD_SHIFT_32;

		// debug
		kernel -> log( (uint8_t *) "[E1000] MMIO base address at 0x%X\n", mmio );

		// remember mmio base address of network controller
		driver_e1000_mmio_base_address = (struct DRIVER_E1000_STRUCTURE_MMIO *) (mmio | KERNEL_PAGE_logical);

		// retrieve interrupt number of network controller
		driver_e1000_irq_number = (uint8_t) driver_pci_read( pci, DRIVER_PCI_REGISTER_irq );

		// map MMIO controller area
		kernel -> page_map( kernel -> page_base_address, (uintptr_t) driver_e1000_mmio_base_address & ~KERNEL_PAGE_logical, (uintptr_t) driver_e1000_mmio_base_address, MACRO_PAGE_ALIGN_UP( STD_PAGE_byte << STD_SHIFT_4 ) >> STD_SHIFT_PAGE, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write );

		// retrieve network controller mac address
		driver_e1000_mac[ 0 ] = (uint8_t) (driver_e1000_eeprom( 0x0001 ) >> 16);
		driver_e1000_mac[ 1 ] = (uint8_t) (driver_e1000_eeprom( 0x0001 ) >> 24);
		driver_e1000_mac[ 2 ] = (uint8_t) (driver_e1000_eeprom( 0x0101 ) >> 16);
		driver_e1000_mac[ 3 ] = (uint8_t) (driver_e1000_eeprom( 0x0101 ) >> 24);
		driver_e1000_mac[ 4 ] = (uint8_t) (driver_e1000_eeprom( 0x0201 ) >> 16);
		driver_e1000_mac[ 5 ] = (uint8_t) (driver_e1000_eeprom( 0x0201 ) >> 24);

		// debug
		kernel -> log( (uint8_t *) "[E1000] MAC address %2X:%2X:%2X:%2X:%2X:%2X\n", driver_e1000_mac[ 0 ], driver_e1000_mac[ 1 ], driver_e1000_mac[ 2 ], driver_e1000_mac[ 3 ], driver_e1000_mac[ 4 ], driver_e1000_mac[ 5 ] );

		// disable all type of interrupts on network controller
		driver_e1000_mmio_base_address -> imc = STD_MAX_unsigned;	// documentation, page 312/410

		// release all pending interrupts
		uint32_t null = driver_e1000_mmio_base_address -> icr;	// documentation, page 307/410

		/*==========================================================================
		= configuration of incoming packets
		==========================================================================*/

		// prepare area for incoming packet descriptors
		driver_e1000_rx_base_address = (struct DRIVER_E1000_STRUCTURE_RDESC *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( DRIVER_E1000_RDLEN_SIZE_byte ) >> STD_SHIFT_PAGE );

		// load descriptor table address to MMIO of network controller
		driver_e1000_mmio_base_address -> rdbal = (uint32_t) ((uintptr_t) driver_e1000_rx_base_address & ~KERNEL_PAGE_logical);
		driver_e1000_mmio_base_address -> rdbah = (uint32_t) (((uintptr_t) driver_e1000_rx_base_address & ~KERNEL_PAGE_logical) >> 32);

		// set up size of descriptor cache, header and limit
		// documentation, page  321/410, point 13.4.27
		// we handle one package at a time
		driver_e1000_mmio_base_address -> rdlen = DRIVER_E1000_RDLEN_default;
		driver_e1000_mmio_base_address -> rdh = 0;	// identificator of first available descriptor on list
		driver_e1000_mmio_base_address -> rdt = 1;	// identificator of end of list of available descriptors

		// insert clean page into first record of incoming packet descriptor table
		driver_e1000_rx_base_address[ 0 ].address = kernel -> memory_alloc_page();

		// configure register of incoming packets
		// documentation, page 314/410, table 13-67
		driver_e1000_mmio_base_address -> rctl = DRIVER_E1000_RCTL_EN | DRIVER_E1000_RCTL_UPE | DRIVER_E1000_RCTL_MPE | DRIVER_E1000_RCTL_BAM | DRIVER_E1000_RCTL_BSIZE_4096_BYTE | DRIVER_E1000_RCTL_BSEX | DRIVER_E1000_RCTL_SECRC;

		/*==========================================================================
		= configuration of outcoming packets
		==========================================================================*/

		// prepare area for outcomming packet descriptors
		driver_e1000_tx_base_address = (struct DRIVER_E1000_STRUCTURE_TDESC *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( DRIVER_E1000_TDLEN_SIZE_byte ) >> STD_SHIFT_PAGE );

		// load descriptor table address to network controller mmio
		driver_e1000_mmio_base_address -> tdbal = (uint32_t) ((uintptr_t) driver_e1000_tx_base_address & ~KERNEL_PAGE_logical);
		driver_e1000_mmio_base_address -> tdbah = (uint32_t) (((uintptr_t) driver_e1000_tx_base_address & ~KERNEL_PAGE_logical) >> 32);

		// set up size of descriptor cache, header and limit
		// documentation, page 330/410, point 13.4.38
		// we handle one package at a time
		driver_e1000_mmio_base_address -> tdlen = DRIVER_E1000_TDLEN_default;
		driver_e1000_mmio_base_address -> tdh = 0;	// identifier of first available descriptor on list
		driver_e1000_mmio_base_address -> tdt = 0;	// identifier of end of list of available descriptors

		// configure register of outcoming packets
		// documentation, page 314/410, table 13-67
		driver_e1000_mmio_base_address -> tctl = DRIVER_E1000_TCTL_EN | DRIVER_E1000_TCTL_PSP | DRIVER_E1000_TCTL_RTLC | DRIVER_E1000_TCTL_CT | DRIVER_E1000_TCTL_COLD;

		/*==========================================================================
		= enable network controller
		==========================================================================*/

		// connect network controller interrupt handler
		kernel -> idt_mount( KERNEL_IDT_IRQ_offset + driver_e1000_irq_number, KERNEL_IDT_TYPE_irq, (uintptr_t) driver_e1000_entry );

		// connect interrupt vector from IDT table to IOAPIC controller
		kernel -> io_apic_connect( KERNEL_IDT_IRQ_offset + driver_e1000_irq_number, KERNEL_IO_APIC_iowin + (driver_e1000_irq_number * 0x02) );

		// set: IPGT, IPGR1, IPGR2
		driver_e1000_mmio_base_address -> tipg = DRIVER_E1000_TIPG_IPGT_DEFAULT | DRIVER_E1000_TIPG_IPGR1_DEFAULT | DRIVER_E1000_TIPG_IPGR2_DEFAULT;

		// clean: LRST, PHY_RST, VME, ILOS, set: SLU, ASDE
		uint32_t ctrl = driver_e1000_mmio_base_address -> ctrl;
		ctrl |= DRIVER_E1000_CTRL_SLU | DRIVER_E1000_CTRL_ASDE;
		ctrl &= ~DRIVER_E1000_CTRL_LRST;
		ctrl &= ~DRIVER_E1000_CTRL_ILOS;
		ctrl &= ~DRIVER_E1000_CTRL_PHY_RST;
		driver_e1000_mmio_base_address -> ctrl = ctrl;

		// enable network controller interrupts
	 	// documentation, page 311/410, point 13.4.20
		driver_e1000_mmio_base_address -> ics = -1;	// debug, DRIVER_E1000_ICS_rxt0 | DRIVER_E1000_ICS_srpd | DRIVER_E1000_ICS_rxo | DRIVER_E1000_ICS_rxdmt0;
	}

	// ultimately, module will check stack of packets to send

	// hold the door
	while( TRUE );
}

__attribute__(( no_caller_saved_registers ))
void driver_e1000( void ) {
	// debug
	kernel -> log( (uint8_t *) "incomming packet.\n" );

	// get network controller status
	volatile uint32_t status = driver_e1000_mmio_base_address -> icr;

	// if descriptor has set Length
	if( driver_e1000_rx_base_address[ 0 ].length ) {
		// descriptor parsed, clean Length
		driver_e1000_rx_base_address[ 0 ].length = EMPTY;
	}
	
	// tell network controller, packets are processed
	driver_e1000_mmio_base_address -> rdh = 0;	// identifier of first available descriptor on list

	// tell APIC of current logical processor that hardware interrupt was handled, propely
	kernel -> lapic_base_address -> eoi = EMPTY;
}