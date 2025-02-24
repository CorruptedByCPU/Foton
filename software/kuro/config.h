/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	#define	KURO_MIMETYPE_up		0x01
	#define	KURO_MIMETYPE_unknown		0x02
	#define	KURO_MIMETYPE_directory		0x03
	#define	KURO_MIMETYPE_link		0x04
	#define	KURO_MIMETYPE_executable	0x05
	#define	KURO_MIMETYPE_library		0x06
	#define	KURO_MIMETYPE_module		0x07
	#define	KURO_MIMETYPE_image		0x08
	#define	KURO_MIMETYPE_plain_text	0x09
	#define	KURO_MIMETYPE_3d_object		0x0A
	#define	KURO_MIMETYPE_memory		0x0B
	#define	KURO_MIMETYPE_disk		0x0C

	void kuro_icon_register( uint8_t type, uint8_t *path );