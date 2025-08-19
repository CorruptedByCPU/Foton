/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void lib_asm_displacement( struct LIB_ASM_STRUCTURE *asm, uint8_t relative ) {
		// value signed?
		uint8_t sign = FALSE;	// no

		// absolute displacement?
		if( ! relative )
			// yes
			log( LIB_ASM_COLOR_IMMEDIATE"0x%8X", (uint32_t) asm -> displacement );
		// no
		else {
			// for 4 Byte value
			if( asm -> displacement < -0xFF || asm -> displacement > 0xFF ) {
				// backward or forward?
				if( asm -> displacement & STD_SIZE_DWORD_sign ) log( LIB_ASM_COLOR_DEFAULT" - "LIB_ASM_COLOR_IMMEDIATE"0x%8X", (uint32_t) -asm -> displacement );
				else log( LIB_ASM_COLOR_DEFAULT" + "LIB_ASM_COLOR_IMMEDIATE"0x%8X", (uint32_t) asm -> displacement );
			// 1 Byte
			} else {
				// backward or forward?
				if( asm -> displacement & STD_SIZE_BYTE_sign ) log( LIB_ASM_COLOR_DEFAULT" - "LIB_ASM_COLOR_IMMEDIATE"0x%2X", (uint8_t) -asm -> displacement );
				else log( LIB_ASM_COLOR_DEFAULT" + "LIB_ASM_COLOR_IMMEDIATE"0x%2X", (uint8_t) asm -> displacement );
			}
		}
}
