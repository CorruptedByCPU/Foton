/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void lib_asm_register( struct LIB_ASM_STRUCTURE *asm, uint8_t operand, uint8_t reg ) {
	// add comma?
	if( asm -> comma_semaphore ) log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR );

	// current bits
	uint8_t bits = asm -> register_bits;

	// strictly definied size for operand?
	if( (! asm -> register_semaphore && operand == 0 && asm -> instruction.options & FO) || (! asm -> memory_semaphore && operand == 1 && asm -> instruction.options & FO) ) {
		if( asm -> instruction.options & (B << (operand * LIB_ASM_OPTION_FLAG_offset)) ) bits = BYTE;
	// 	if( asm -> instruction.options & (W << (operand * LIB_ASM_OPTION_FLAG_offset)) ) bits = WORD;
		if( asm -> instruction.options & (D << (operand * LIB_ASM_OPTION_FLAG_offset)) ) bits = DWORD;
	// 	if( asm -> instruction.options & (Q << (operand * LIB_ASM_OPTION_FLAG_offset)) ) bits = QWORD;
	}

	// register size override by REX for specified operand?
	if( ! asm -> register_semaphore && operand == 0 && asm -> rex.w ) bits = QWORD;	// forced 64 bit
	if( ! asm -> memory_semaphore && operand == 1 && asm -> rex.w ) bits = QWORD;	// forced 64 bit

	// if there is no 64 bit extension, and we use 8 bit registers
	if( ! asm -> rex.semaphore && ! bits ) log( LIB_ASM_COLOR_REGISTER"%s", r_no_rex[ reg ] );	// use special array
	
	// otherwise

	// default array
	else log( LIB_ASM_COLOR_REGISTER"%s", r[ bits ][ reg ] );
}
