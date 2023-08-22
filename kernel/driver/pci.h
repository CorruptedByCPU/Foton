/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef DRIVER_PCI
        #define DRIVER_PCI

        #define	DRIVER_PCI_PORT_command			0x0CF8
        #define	DRIVER_PCI_PORT_data			0x0CFC

        #define	DRIVER_PCI_REGISTER_vendor_and_device	0x00
        #define	DRIVER_PCI_REGISTER_status_and_command	0x04
        #define	DRIVER_PCI_REGISTER_class_and_subclass	0x08
        #define	DRIVER_PCI_REGISTER_bar0		0x10
        #define	DRIVER_PCI_REGISTER_bar1		0x14
        #define	DRIVER_PCI_REGISTER_bar2		0x18
        #define	DRIVER_PCI_REGISTER_bar3		0x1C
        #define	DRIVER_PCI_REGISTER_bar4		0x20
        #define	DRIVER_PCI_REGISTER_bar5		0x24
        #define	DRIVER_PCI_REGISTER_irq			0x3C
        #define	DRIVER_PCI_REGISTER_FLAG_64		0b00000010

        #define	DRIVER_PCI_CLASS_SUBCLASS_ide		0x0101
        #define	DRIVER_PCI_CLASS_SUBCLASS_ahci		0x0106
        #define	DRIVER_PCI_CLASS_SUBCLASS_scsi		0x0107
        #define	DRIVER_PCI_CLASS_SUBCLASS_network	0x0200

        struct	DRIVER_PCI_STRUCTURE {
                uint16_t	result;
                uint32_t	bus;
                uint32_t	device;
                uint32_t	function;
        };

        uint32_t driver_pci_read( uint32_t reg, struct DRIVER_PCI_STRUCTURE pci );
        struct DRIVER_PCI_STRUCTURE driver_pci_find_class_and_subclass( uint16_t class_and_subclass );
#endif