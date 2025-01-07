/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	#define	KURO_ICON_TYPE_up		0x01
	#define	KURO_ICON_TYPE_default		0x02
	#define	KURO_ICON_TYPE_directory	0x03
	#define	KURO_ICON_TYPE_link		0x04
	#define	KURO_ICON_TYPE_executable	0x05
	#define	KURO_ICON_TYPE_library		0x06
	#define	KURO_ICON_TYPE_module		0x07
	#define	KURO_ICON_TYPE_image		0x08

	void kuro_icon_register( uint8_t type, uint8_t *path );