/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t lib_asm_name( struct LIB_ASM_STRUCTURE *asm ) {
	#ifdef LIB_ASM_OPCODE
		log( LIB_ASM_COLOR_DEFAULT"[%2X] ", asm -> opcode );
	#endif

	// special case of name?
	if( asm -> instruction.name && asm -> instruction.group ) {
		// properties of names
		struct LIB_ASM_STRUCTURE_INSTRUCTION *group = asm -> instruction.group;

		// forced use of 64 bit?
		if( asm -> rex.w ) asm -> register_bits = QWORD;

		// exception for opcode: 0xE3
		if( asm -> opcode == 0xE3 )
			// select name
			log( LIB_ASM_COLOR_INSTRUCTION"%s"LIB_ASM_SEPARATOR, group[ asm -> memory_bits ] );

		// select name
		else log( LIB_ASM_COLOR_INSTRUCTION"%s"LIB_ASM_SEPARATOR, group[ asm -> register_bits ] );

		// done
		return TRUE;
	}

	// instruction name
	uint8_t *name = asm -> instruction.name;

	// if doesn't exist
	if( ! name )
		// retrieve name from group
		name = ((struct LIB_ASM_STRUCTURE_INSTRUCTION *) asm -> instruction.group)[ asm -> modrm.reg ].name;

	// name still doesn't exist?
	if( ! name ) { log( "#UD" ); return FALSE; }

	// deBuffer?
	if( asm -> opcode == 0xCC ) { log( LIB_ASM_COLOR_INSTRUCTION"int"LIB_ASM_SEPARATOR""LIB_ASM_COLOR_DATA"deBuffer" ); return TRUE; }

	// show instruction name

	// exception for INSD mnemonic
	if( asm -> opcode == 0x6D && asm -> register_bits == DWORD ) log( LIB_ASM_COLOR_INSTRUCTION"insd" );
		
	// exception for OUTSD mnemonic
	else if( asm -> opcode == 0x6F && asm -> register_bits == DWORD ) log( LIB_ASM_COLOR_INSTRUCTION"outsd" );

	// default
	else log( LIB_ASM_COLOR_INSTRUCTION"%s"LIB_ASM_SEPARATOR, name );

	// done
	return TRUE;
}
