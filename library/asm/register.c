/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t *lib_asm_register( struct LIB_ASM_STRUCTURE *asm, uint8_t operand, uint8_t reg ) {
	// current bits
	uint8_t bits = asm -> reg_bits;

	// register size override?
	if( asm -> prefix == 0x66 ) bits = 1;	// 16 bit
	if( asm -> prefix == 0x67 ) bits = 2;	// 32 bit
	
	// or by REX (which have highest priority)
	if( asm -> rex.w ) bits = 3;	// forced 64 bit

	// strictly definied size for first operand?
	if( asm -> instruction.options & F1 ) {
		if( asm -> instruction.options & (B << (operand * LIB_ASM_OPTION_FLAG_offset)) ) bits = 0;	// 8 bit
		if( asm -> instruction.options & (W << (operand * LIB_ASM_OPTION_FLAG_offset)) ) bits = 1;	// 16 bit
		if( asm -> instruction.options & (D << (operand * LIB_ASM_OPTION_FLAG_offset)) ) bits = 2;	// 32 bit
		if( asm -> instruction.options & (Q << (operand * LIB_ASM_OPTION_FLAG_offset)) ) bits = 3;	// 64 bit
	}

	// strictly definied size for second operand?
	if( asm -> instruction.options & F2 ) {
		if( asm -> instruction.options & (B << (operand * LIB_ASM_OPTION_FLAG_offset)) ) bits = 0;	// 8 bit
		if( asm -> instruction.options & (W << (operand * LIB_ASM_OPTION_FLAG_offset)) ) bits = 1;	// 16 bit
		if( asm -> instruction.options & (D << (operand * LIB_ASM_OPTION_FLAG_offset)) ) bits = 2;	// 32 bit
		if( asm -> instruction.options & (Q << (operand * LIB_ASM_OPTION_FLAG_offset)) ) bits = 3;	// 64 bit
	}

	// if there is no 64 bit extension, and we use 8 bit registers
	if( ! asm -> rex_semaphore && ! bits ) return r_no_rex[ reg ];	// use special array
	
	// otherwise

	// default array
	return r[ bits ][ reg ];
}
