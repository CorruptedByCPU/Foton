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

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// find device of type "network"
	struct DRIVER_PCI_STRUCTURE pci = driver_pci_find_class_and_subclass( DRIVER_PCI_CLASS_SUBCLASS_network );

	// if found, check model and vendor
	if( driver_pci_read( pci, DRIVER_PCI_REGISTER_vendor_and_device ) == MODULE_E1000_VENDOR_AND_DEVICE ) {
		// debug
		// kernel -> log( (uint8_t *) "[E1000] PCI %2X:%2X.%u - Network Controller found.\n", pci.bus, pci.device, pci.function );

		// get base address of network controller mmio
		uint64_t mmio = (uint64_t) driver_pci_read( pci, DRIVER_PCI_REGISTER_bar0 );

		// unsupported method?
		if( (uint8_t) mmio & DRIVER_PCI_REGISTER_BAR0_FLAG_io ) {
			// initialization halted
			// kernel -> log( (uint8_t *) "[E1000] I/O is unsupported.\n" );

			// hold the door
			while( TRUE );
		}

		// base address is 64 bit wide?
		if( (uint8_t) mmio & DRIVER_PCI_REGISTER_BAR0_FLAG_64bit )
			// get older 32 bit part of full address
			mmio |= (uint64_t) driver_pci_read( pci, DRIVER_PCI_REGISTER_bar1 ) << STD_SHIFT_32;

		// remember mmio base address of network controller
		module_e1000_mmio_base_address = (struct MODULE_E1000_STRUCTURE_MMIO *) ((mmio & ~0x0F) | KERNEL_PAGE_mirror);

		// debug
		// kernel -> log( (uint8_t *) "[E1000] MMIO base address at 0x%X\n", module_e1000_mmio_base_address );

		// retrieve interrupt number of network controller
		module_e1000_irq_number = (uint8_t) driver_pci_read( pci, DRIVER_PCI_REGISTER_irq );

		// map MMIO controller area
		kernel -> page_map( kernel -> page_base_address, (uintptr_t) module_e1000_mmio_base_address & ~KERNEL_PAGE_mirror, (uintptr_t) module_e1000_mmio_base_address, MACRO_PAGE_ALIGN_UP( MODULE_E1000_MMIO_limit ) >> STD_SHIFT_PAGE, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write );

		// retrieve network controller mac address
		module_e1000_mac[ 0 ] = (uint8_t) (module_e1000_eeprom( 0x0001 ) >> 16);
		module_e1000_mac[ 1 ] = (uint8_t) (module_e1000_eeprom( 0x0001 ) >> 24);
		module_e1000_mac[ 2 ] = (uint8_t) (module_e1000_eeprom( 0x0101 ) >> 16);
		module_e1000_mac[ 3 ] = (uint8_t) (module_e1000_eeprom( 0x0101 ) >> 24);
		module_e1000_mac[ 4 ] = (uint8_t) (module_e1000_eeprom( 0x0201 ) >> 16);
		module_e1000_mac[ 5 ] = (uint8_t) (module_e1000_eeprom( 0x0201 ) >> 24);

		// share interaface MAC address with kernel
		for( uint8_t i = 0; i < 6; i++ ) kernel -> network_interface.ethernet_address[ i ] = module_e1000_mac[ i ];

		// debug
		// kernel -> log( (uint8_t *) "[E1000] MAC address %2X:%2X:%2X:%2X:%2X:%2X\n", module_e1000_mac[ 0 ], module_e1000_mac[ 1 ], module_e1000_mac[ 2 ], module_e1000_mac[ 3 ], module_e1000_mac[ 4 ], module_e1000_mac[ 5 ] );

		// disable all type of interrupts on network controller
		module_e1000_mmio_base_address -> imc = STD_MAX_unsigned;	// documentation, page 312/410

		// release all pending interrupts
		volatile uint32_t null = module_e1000_mmio_base_address -> icr;	// documentation, page 307/410

		/*==========================================================================
		= configuration of incoming packets
		==========================================================================*/

		// prepare area for incoming packet descriptors
		module_e1000_rx_base_address = (struct MODULE_E1000_STRUCTURE_RDESC *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( MODULE_E1000_RDLEN_SIZE_byte ) >> STD_SHIFT_PAGE );

		// load descriptor table address to MMIO of network controller
		module_e1000_mmio_base_address -> rdbal = (uint32_t) ((uintptr_t) module_e1000_rx_base_address & ~KERNEL_PAGE_mirror);
		module_e1000_mmio_base_address -> rdbah = (uint32_t) (((uintptr_t) module_e1000_rx_base_address & ~KERNEL_PAGE_mirror) >> 32);

		// set up size of descriptor cache, header and limit
		// documentation, page  321/410, point 13.4.27
		// we handle one package at a time
		module_e1000_mmio_base_address -> rdlen = MODULE_E1000_RDLEN_default;
		module_e1000_mmio_base_address -> rdh = 0;	// identificator of first available descriptor on list
		module_e1000_mmio_base_address -> rdt = 1;	// identificator of end of list of available descriptors

		// insert clean page into first record of incoming packet descriptor table
		module_e1000_rx_base_address[ 0 ].address = kernel -> memory_alloc_page();

		// configure register of incoming packets
		// documentation, page 314/410, table 13-67
		module_e1000_mmio_base_address -> rctl = MODULE_E1000_RCTL_EN | MODULE_E1000_RCTL_UPE | MODULE_E1000_RCTL_MPE | MODULE_E1000_RCTL_BAM | MODULE_E1000_RCTL_SECRC;

		/*==========================================================================
		= configuration of outcoming packets
		==========================================================================*/

		// prepare area for outcomming packet descriptors
		module_e1000_tx_base_address = (struct MODULE_E1000_STRUCTURE_TDESC *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( MODULE_E1000_TDLEN_SIZE_byte ) >> STD_SHIFT_PAGE );

		// load descriptor table address to network controller mmio
		module_e1000_mmio_base_address -> tdbal = (uint32_t) ((uintptr_t) module_e1000_tx_base_address);
		module_e1000_mmio_base_address -> tdbah = (uint32_t) (((uintptr_t) module_e1000_tx_base_address & ~KERNEL_PAGE_mirror) >> 32);

		// set up size of descriptor cache, header and limit
		// documentation, page 330/410, point 13.4.38
		// we handle one package at a time
		module_e1000_mmio_base_address -> tdlen = MODULE_E1000_TDLEN_default;
		module_e1000_mmio_base_address -> tdh = 0;	// identifier of first available descriptor on list
		module_e1000_mmio_base_address -> tdt = 0;	// identifier of end of list of available descriptors

		// configure register of outcoming packets
		// documentation, page 314/410, table 13-67
		module_e1000_mmio_base_address -> tctl = MODULE_E1000_TCTL_EN | MODULE_E1000_TCTL_PSP | MODULE_E1000_TCTL_RTLC | MODULE_E1000_TCTL_CT | MODULE_E1000_TCTL_COLD;

		/*==========================================================================
		= enable network controller
		==========================================================================*/

		// connect network controller interrupt handler
		kernel -> idt_mount( KERNEL_IDT_IRQ_offset + module_e1000_irq_number, KERNEL_IDT_TYPE_irq, (uintptr_t) module_e1000_entry );

		// connect interrupt vector from IDT table to IOAPIC controller
		kernel -> io_apic_connect( KERNEL_IDT_IRQ_offset + module_e1000_irq_number, KERNEL_IO_APIC_iowin + (module_e1000_irq_number * 0x02) );

		// set: IPGT, IPGR1, IPGR2
		module_e1000_mmio_base_address -> tipg = MODULE_E1000_TIPG_IPGT_DEFAULT | MODULE_E1000_TIPG_IPGR1_DEFAULT | MODULE_E1000_TIPG_IPGR2_DEFAULT;

		// clean: LRST, PHY_RST, VME, ILOS, set: SLU, ASDE
		volatile uint32_t ctrl = module_e1000_mmio_base_address -> ctrl;
		ctrl |= MODULE_E1000_CTRL_SLU | MODULE_E1000_CTRL_ASDE;
		ctrl &= ~MODULE_E1000_CTRL_LRST;
		ctrl &= ~MODULE_E1000_CTRL_ILOS;
		ctrl &= ~MODULE_E1000_CTRL_VME;
		ctrl &= ~MODULE_E1000_CTRL_PHY_RST;
		module_e1000_mmio_base_address -> ctrl = ctrl;

		// enable network controller interrupts
	 	// documentation, page 311/410, point 13.4.20
		module_e1000_mmio_base_address -> ics = MODULE_E1000_ICS_rxt0 | MODULE_E1000_ICS_srpd | MODULE_E1000_ICS_rxo | MODULE_E1000_ICS_rxdmt0;
	}

	// ultimately, module will check stack of packets to send

	// transmit function ready?
	while( ! kernel -> network_tx );

	// hold the door
	while( TRUE ) {
		// properties of frame to send
		uintptr_t frame = EMPTY;

		// acquire data for transmission
		while( ! (frame = kernel -> network_tx()) ) kernel -> time_sleep( TRUE );

		// resolve properties
		uintptr_t data = frame & STD_PAGE_mask;
		uint64_t length = frame & ~STD_PAGE_mask;

		// load data pointer
		module_e1000_tx_base_address[ 0 ].address = data;

		// set flags and length of data
		module_e1000_tx_base_address[ 0 ].length = length;
		module_e1000_tx_base_address[ 0 ].command = MODULE_E1000_TDESC_CMD_EOP | MODULE_E1000_TDESC_CMD_IFCS | MODULE_E1000_TDESC_CMD_RS;

		// inform controller about data to send
		module_e1000_mmio_base_address -> tdh = 0;
		module_e1000_mmio_base_address -> tdt = 1;

		// frame was sent?
		while( module_e1000_mmio_base_address -> tdh == 0 && module_e1000_mmio_base_address -> tdt == 1 );

		// release page
		kernel -> memory_release_page( data );

		// debug
		kernel -> log( (uint8_t *) "Tx\n" );
	}
}

__attribute__(( preserve_most ))
void driver_e1000( void ) {
	// get network controller status
	volatile uint32_t status = module_e1000_mmio_base_address -> status;

	// get network controller "interrupt due to"
	volatile uint32_t interrupt_cause = module_e1000_mmio_base_address -> icr;

	// if descriptor has set Length
	if( module_e1000_rx_base_address[ 0 ].length ) {
		// storage function of network is available
		kernel -> network_rx( (uintptr_t) module_e1000_rx_base_address[ 0 ].address | module_e1000_rx_base_address[ 0 ].length | KERNEL_PAGE_mirror );

		// insert clean page
		module_e1000_rx_base_address[ 0 ].address = kernel -> memory_alloc_page();

		// tell network controller, packets are processed
		module_e1000_mmio_base_address -> rdh = 0;	// identifier of first available descriptor on list

		// debug
		kernel -> log( (uint8_t *) "Rx\n" );
	}

	// tell APIC of current logical processor that hardware interrupt was handled, propely
	kernel -> lapic_base_address -> eoi = EMPTY;
}

uint32_t module_e1000_eeprom( uint32_t reg ) {
	// ask about register content
	module_e1000_mmio_base_address -> eerd = reg;

	// return its value
	return module_e1000_mmio_base_address -> eerd;
}