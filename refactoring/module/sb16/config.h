/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	MODULE_SB16
	#define	MODULE_SB16

	#define	MODULE_SB16_PORT_MIXER			0x0224
	#define	MODULE_SB16_PORT_DATA			0x0225
	#define MODULE_SB16_PORT_RESET			0x0226
	#define MODULE_SB16_PORT_READ			0x022A
	#define MODULE_SB16_PORT_WRITE			0x022C
	#define MODULE_SB16_PORT_STATUS			0x022E
	#define MODULE_SB16_PORT_ACK_16			0x022F

	#define	MODULE_SB16_COMMAND_speaker_on		0xD1
	#define	MODULE_SB16_COMMAND_speaker_off		0xD3
	#define	MODULE_SB16_COMMAND_16bit_channel_stop	0xD5
	#define	MODULE_SB16_COMMAND_16bit_channel_start	0xD6
#endif