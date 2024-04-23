/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct KERNEL *kernel = EMPTY;

volatile struct MODULE_E1000_STRUCTURE_MMIO	*module_e1000_mmio_base_address;
volatile struct MODULE_E1000_STRUCTURE_RDESC	*module_e1000_rx_base_address;
volatile struct MODULE_E1000_STRUCTURE_TDESC	*module_e1000_tx_base_address;

uint8_t module_e1000_irq_number = EMPTY;

uint8_t module_e1000_mac[ 6 ]		= { EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY };
uint8_t module_e1000_mac_broadcast[ 6 ]	= { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };