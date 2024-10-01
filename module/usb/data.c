/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct KERNEL *kernel = EMPTY;

size_t module_usb_controller_limit = EMPTY;

struct MODULE_USB_STRUCTURE_CONTROLLER *module_usb_controller = EMPTY;
struct MODULE_USB_STRUCTURE_PORT *module_usb_port = EMPTY;

struct MODULE_USB_STRUCTURE_UHCI_QUEUE *module_usb_uhci_queue_1u	= EMPTY;
struct MODULE_USB_STRUCTURE_UHCI_QUEUE *module_usb_uhci_queue_8u	= EMPTY;
struct MODULE_USB_STRUCTURE_UHCI_QUEUE *module_usb_uhci_queue_16u	= EMPTY;