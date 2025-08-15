/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void lib_asm_name( struct LIB_ASM_STRUCTURE *asm ) {
	#ifdef LIB_ASM_OPCODE
		log( LIB_ASM_COLOR_DEFAULT"[%2X] ", asm -> opcode );
	#endif

	// special case of name?
	if( asm -> instruction.name && asm -> instruction.group ) {
		// properties of names
		struct LIB_ASM_STRUCTURE_INSTRUCTION *group = asm -> instruction.group;

		// forced use of 64 bit?
		if( asm -> rex.w ) asm -> register_bits = QWORD;

		// select name
		log( LIB_ASM_COLOR_INSTRUCTION"%s", group[ asm -> register_bits ] );

		// done
		return;
	}

	// instruction name
	uint8_t *name = asm -> instruction.name;

	// if doesn't exist
	if( ! name )
		// retrieve name from group
		name = ((struct LIB_ASM_STRUCTURE_INSTRUCTION *) asm -> instruction.group)[ asm -> modrm.reg ].name;

	// show instruction name

	// exception for INSD mnemonic
	if( asm -> opcode == 0x6D && asm -> register_bits == DWORD ) log( LIB_ASM_COLOR_INSTRUCTION"insd" );
		
	// exception for OUTSD mnemonic
	else if( asm -> opcode == 0x6F && asm -> register_bits == DWORD ) log( LIB_ASM_COLOR_INSTRUCTION"outsd" );

	// default
	else log( LIB_ASM_COLOR_INSTRUCTION"%s"LIB_ASM_SEPARATOR, name );
}
