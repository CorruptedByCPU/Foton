/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct KERNEL *kernel = EMPTY;

uint8_t module_usb_string_port[] = "port";
uint8_t module_usb_string_memory[] = "memory";

struct MODULE_USB_CONTROLLER_STRUCTURE *module_usb_controller = EMPTY;
struct MODULE_USB_PORT_STRUCTURE *module_usb_port = EMPTY;

uint8_t module_usb_controller_count = EMPTY;
uint8_t	module_usb_port_count = 1;	// 0th entry is reserved, others mean as device assigned enumeration

struct MODULE_USB_QUEUE_STRUCTURE *module_usb_queue_1ms = EMPTY;