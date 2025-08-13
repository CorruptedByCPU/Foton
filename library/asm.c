/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// #define DEBUF

	//----------------------------------------------------------------------
	// static, structures, definitions
	//----------------------------------------------------------------------
	#include	"./asm/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./asm/data.c"
	//----------------------------------------------------------------------
	// routines, procedures
	//----------------------------------------------------------------------
	#include	"./asm/init.c"
	#include	"./asm/displacement.c"
	#include	"./asm/modrm.c"
	#include	"./asm/name.c"
	#include	"./asm/immediate.c"
	#include	"./asm/memory.c"
	#include	"./asm/register.c"
	//----------------------------------------------------------------------

// output to stdout, and return amount of parsed Bytes 
uint64_t lib_asm( void *rip ) {
	// one-time global environment initialization
	struct LIB_ASM_STRUCTURE asm_variables = { EMPTY }; struct LIB_ASM_STRUCTURE *asm = (struct LIB_ASM_STRUCTURE *) &asm_variables; asm -> rip = rip; uint8_t ready = lib_asm_init( asm );

	// are we ready for interpretation?
	if( ! ready ) {	// nope
		// message
		log( "invalid opcode (0x%2X) or data", asm -> opcode );

		// we are done
		goto end;
	}

	// instruction name
	lib_asm_name( asm );
	
	// only instruction name?
	// if( ! asm -> instruction.options ) goto end;	// yes

	// for instructions belongs to group
	if( asm -> instruction.group ) {
	} else {
		// try to parse ModR/M, if exist
		if( ! lib_asm_modrm( asm ) ) {	// doesn't
			// first operand is a register?
			if( asm -> instruction.options & R ) {
				// show
				lib_asm_register( asm, 0, asm -> modrm.reg | (asm -> rex.r << 3) );

				// separator required from now on
				asm -> comma_semaphore = TRUE;
			} else {
				// might be a hidden inside opcode (0x50-0x5F)
				if( asm -> instruction.options & FH ) {
					// only 64 bit registers allowed
					asm -> register_bits = QWORD;

					// show
					lib_asm_register( asm, 0, asm -> opcode & STD_MASK_byte_half | (asm -> rex.b << 3) );

					// end
					goto end;
				}
			}
		}
	}

	// check for any left operand, which can be immediate
	lib_asm_immediate( asm );

//		// by default, check instruction with ModR/M existence
// 		if( ! lib_asm_modrm( asm ) ) {
// 			// only first operand is a register?
// 			if( asm -> instruction.options & R ) {
// 				// show
// 				log( "\033[38;2;255;166;87m%s", lib_asm_register( asm, 0, asm -> modrm.reg | (asm -> rex.r << 3) ) );
			
// 				// first operand column filled
// 				asm -> col++;
// 			// no, there is no register in any operand
// 			} else {
// 				// might be hidden inside opcode (0x50-0x5F)
// 				if( asm -> instruction.options & FR ) {
// 					// only 64 bit registers allowed
// 					asm -> register_bits = QWORD;

// 					log( "\033[38;2;255;166;87m\0" );

// 					// show
// 					log( "%s", lib_asm_register( asm, 0, asm -> opcode & STD_MASK_byte_half | (asm -> rex.b << 3) ) );

// 					// end
// 					goto end;
// 				}
// 			}
// 		}
// 	// parse group existence
// 	} else {
// 		// start memory access
// 		log( "\033[38;5;202m[" );

// 		// show register
// 		log( "\033[38;2;255;166;87m%s", r[ asm -> mem_bits ][ asm -> modrm.rm ] );

// 		// displacement exist?
// 		if( asm -> displacement ) log( "\033[0m + \033[38;2;121;192;255m0x" );

// 		// yes
// 		if( asm -> displacement > 0xFFFF ) log( "%8X", asm -> displacement );
// 		else if( asm -> displacement > 0xFF ) log( "%4X", asm -> displacement );
// 		else if( asm -> displacement ) log( "%2X", asm -> displacement );

// 		// end memory access
// 		log( "\033[38;5;202m]" );

// 		// first operand column filled
// 		asm -> col++;
// 	}

// 	// check for any left operand, which can be immediate
// 	lib_asm_immediate( asm );

end:
	// amount of parsed Bytes
	return (uintptr_t) asm -> rip - (uintptr_t) rip;
}
