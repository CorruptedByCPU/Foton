/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t lib_asm_name( struct LIB_ASM_STRUCTURE *asm ) {
	#ifdef LIB_ASM_OPCODE
		log( LIB_ASM_COLOR_DEFAULT"[%2X] ", asm -> opcode );
	#endif

	// special case of name? (with options)
	if( asm -> instruction.name && ! asm -> instruction.options && asm -> instruction.group ) {
		// properties of names
		struct LIB_ASM_STRUCTURE_INSTRUCTION *group = asm -> instruction.group;

		// current bits
		uint8_t bits = asm -> register_bits;

		// change to 16 bit mode?
		if( asm -> register_semaphore ) bits = WORD;

		// change to 64 bit mode?
		if( ! asm -> register_semaphore && asm -> rex.w ) bits = QWORD;

		// select name
		log( LIB_ASM_COLOR_INSTRUCTION"%s"LIB_ASM_SEPARATOR, group[ bits ] );

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
	log( LIB_ASM_COLOR_INSTRUCTION"%s"LIB_ASM_SEPARATOR, name );

	// done
	return TRUE;
}
