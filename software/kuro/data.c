/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

MACRO_IMPORT_FILE_AS_ARRAY( interface, "./software/kuro/interface.json" )

struct LIB_INTERFACE_STRUCTURE			*kuro_interface;
struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE	*kuro_files;
struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE	*kuro_storages;

uint32_t **kuro_icons = EMPTY;
uint8_t	kuro_key_ctrl_semaphore = FALSE;
