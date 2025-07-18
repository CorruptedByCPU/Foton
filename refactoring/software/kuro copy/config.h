/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	KURO_FILE_EXTENSION_LENGTH_3	3
#define	KURO_FILE_EXTENSION_LENGTH_4	4

#define	KURO_MIMETYPE_unknown		0x00
#define	KURO_MIMETYPE_up		0x01
#define	KURO_MIMETYPE_directory		0x02
#define	KURO_MIMETYPE_link		0x03
#define	KURO_MIMETYPE_executable	0x04
#define	KURO_MIMETYPE_library		0x05
#define	KURO_MIMETYPE_module		0x06
#define	KURO_MIMETYPE_image		0x07
#define	KURO_MIMETYPE_plain_text	0x08
#define	KURO_MIMETYPE_3d_object		0x09
#define	KURO_MIMETYPE_memory		0x0A
#define	KURO_MIMETYPE_disk		0x0B
#define	KURO_MIMETYPE_log		0x0C

#define	KURO_LIST_COLOR_default		0xFF141414
#define	KURO_LIST_COLOR_odd		KURO_LIST_COLOR_default
#define	KURO_LIST_COLOR_selected	0xFF202020

void kuro_icons_register( uint8_t type, uint8_t *path );
