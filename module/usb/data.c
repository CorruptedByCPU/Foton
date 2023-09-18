/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct KERNEL *kernel = EMPTY;

uint8_t driver_usb_string_port[] = "port";
uint8_t driver_usb_string_memory[] = "memory";

struct DRIVER_USB_CONTROLLER_STRUCTURE *driver_usb_controller = EMPTY;
struct DRIVER_USB_PORT_STRUCTURE *driver_usb_port = EMPTY;

uint8_t driver_usb_controller_count = EMPTY;
uint8_t	driver_usb_port_count = EMPTY;

struct DRIVER_USB_QUEUE_STRUCTURE *driver_usb_queue_1ms = EMPTY;