/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void lib_asm_immediate( struct LIB_ASM_STRUCTURE *asm ) {
	// 8 bit value?
	if( asm -> instruction.options & (B << LIB_ASM_OPTION_FLAG_2nd_operand_shift) )
		// show 1 Byte
		log( "0x%2X", (uint8_t) *(asm -> rip++) );
	
	// 32 bit value?
	if( asm -> instruction.options & (D << LIB_ASM_OPTION_FLAG_3rd_operand_shift) ) {
		// acquire 4 Bytes
		uint32_t immediate = *((uint32_t *) asm -> rip); asm -> rip += 4;

		// show
		log( "0x%8X", immediate );
	}
}
