/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef DRIVER_PCI
        #define DRIVER_PCI

        #define	DRIVER_PCI_PORT_command				0x0CF8
        #define	DRIVER_PCI_PORT_data				0x0CFC

        #define	DRIVER_PCI_REGISTER_vendor_and_device		0x00
        #define	DRIVER_PCI_REGISTER_status_and_command		0x04
        #define	DRIVER_PCI_REGISTER_class_and_subclass		0x08
        #define	DRIVER_PCI_REGISTER_bar0			0x10
        #define	DRIVER_PCI_REGISTER_bar1			0x14
        #define	DRIVER_PCI_REGISTER_bar2			0x18
        #define	DRIVER_PCI_REGISTER_bar3			0x1C
        #define	DRIVER_PCI_REGISTER_bar4			0x20
        #define	DRIVER_PCI_REGISTER_bar5			0x24
        #define	DRIVER_PCI_REGISTER_irq				0x3C
	#define	DRIVER_PCI_REGISTER_BAR0_FLAG_io		0b00000001
        #define	DRIVER_PCI_REGISTER_BAR0_FLAG_64bit		0b00000100

	#define	DRIVER_PCI_REGISTER_CONTROL_IO_SPACE		(1 << 0)
	#define	DRIVER_PCI_REGISTER_CONTROL_MEMORY_SPACE	(1 << 1)
	#define	DRIVER_PCI_REGISTER_CONTROL_BUS_MASTER		(1 << 2)
	#define	DRIVER_PCI_REGISTER_CONTROL_IRQ_DISABLE		(1 << 10)

        #define	DRIVER_PCI_CLASS_SUBCLASS_ide			0x0101
        #define	DRIVER_PCI_CLASS_SUBCLASS_ahci			0x0106
        #define	DRIVER_PCI_CLASS_SUBCLASS_scsi			0x0107
	#define	DRIVER_PCI_CLASS_SUBCLASS_nvme			0x0108
        #define	DRIVER_PCI_CLASS_SUBCLASS_network		0x0200
	#define	DRIVER_PCI_CLASS_SUBCLASS_audio			0x0401
	#define	DRIVER_PCI_CLASS_SUBCLASS_usb			0x0C03

        struct	DRIVER_PCI_STRUCTURE {
                uint16_t	result;
                uint8_t		bus;
                uint8_t		device;
                uint8_t		function;
        };

	// read 32 bit value from PCI register of definied bus:device:function
        uint32_t driver_pci_read( struct DRIVER_PCI_STRUCTURE pci, uint32_t reg );

	// write 32 bit value to PCI register of definied bus:device:function
	void driver_pci_write( struct DRIVER_PCI_STRUCTURE pci, uint32_t reg, uint32_t value );

	// find bus:device:function by provided CLASS:SUBCLASS
        struct DRIVER_PCI_STRUCTURE driver_pci_find_class_and_subclass( uint16_t class_and_subclass );
#endif