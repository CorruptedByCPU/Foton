/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

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
	#include	"./asm/memory.c"
	#include	"./asm/register.c"
	#include	"./asm/value.c"
	//----------------------------------------------------------------------

// output to stdout, and return amount of parsed Bytes 
uint64_t lib_asm( void *rip ) {
	// one-time global environment initialization
	struct LIB_ASM_STRUCTURE asm_variables = { EMPTY }; struct LIB_ASM_STRUCTURE *asm = (struct LIB_ASM_STRUCTURE *) &asm_variables; asm -> rip = rip; uint8_t ready = lib_asm_init( asm );

	// are we ready for interpretation?
	if( ! ready ) {	// nope
		// message
		log( LIB_ASM_COLOR_INSTRUCTION"db " ); if( asm -> extended ) log( LIB_ASM_COLOR_DATA"0x0F, " );
		log( LIB_ASM_COLOR_DATA"0x%2X", asm -> opcode );

		// we are done
		goto end;
	}

	// instruction name
	if( ! lib_asm_name( asm ) ) goto end;
	
	// only instruction name?
	if( ! asm -> instruction.options ) goto end;	// yes

	// if ModR/M exist, parse
	if( asm -> modrm.semaphore ) lib_asm_modrm( asm );

	// no
	else {
		// check for special flags
		
		// register definied by instruction
		if( asm -> instruction.options & FD ) {
			// out
			if( asm -> opcode == 0xE6 ) {
				// retrieve value
				lib_asm_value( asm );

				// show supplementary register
				log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR""LIB_ASM_COLOR_REGISTER"al" );

				// done
				goto end;
			}

			// out
			if( asm -> opcode == 0xE7 ) {
				// retrieve value
				lib_asm_value( asm );

				// current bits
				uint8_t bits = asm -> register_bits;

				// change to 16 bit mode?
				if( asm -> register_semaphore ) bits = WORD;

				// show supplementary register (AX, EAX)
				log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR""LIB_ASM_COLOR_REGISTER"%s", lib_asm_registers[ bits ][ 0 ] );

				// done
				goto end;
			}

			// in
			if( asm -> opcode == 0xEC ) {
				// default registers
				log( LIB_ASM_COLOR_REGISTER"al"LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR""LIB_ASM_COLOR_REGISTER"dx" );

				// done
				goto end;
			}

			// in
			if( asm -> opcode == 0xED ) {
				// current bits
				uint8_t bits = asm -> register_bits;

				// change to 16 bit mode?
				if( asm -> register_semaphore ) bits = WORD;

				// destination address
				log( LIB_ASM_COLOR_REGISTER"%s", lib_asm_registers[ bits ][ 0 ] );

				// source register
				log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR""LIB_ASM_COLOR_REGISTER"dx" );

				// done
				goto end;
			}

			// out
			if( asm -> opcode == 0xEE ) {
				// default register
				log( LIB_ASM_COLOR_REGISTER"dx"LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR""LIB_ASM_COLOR_REGISTER"al" );

				// done
				goto end;
			}

			// out
			if( asm -> opcode == 0xEF ) {
				// destination address
				log( LIB_ASM_COLOR_REGISTER"dx" );

				// current bits
				uint8_t bits = asm -> register_bits;

				// change to 16 bit mode?
				if( asm -> register_semaphore ) bits = WORD;

				// source register
				log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR""LIB_ASM_COLOR_REGISTER"%s", lib_asm_registers[ bits ][ 0 ] );

				// done
				goto end;
			}

			// register id
			uint8_t register_id = asm -> opcode & STD_MASK_byte_half;

			// instruction alignment
			if( register_id > 0x07 ) register_id -= 0x08;

			// instruction type
			switch(	asm -> opcode & ~STD_MASK_byte_half ) {
				// pop
				case 0x50: {
					// only 64 bit registers are allowed
					asm -> register_bits = QWORD;
				}

				default: {
					// show
					lib_asm_register( asm, 0, register_id | (asm -> rex.b << 3) );
				}
			}

			// xchg (0x90-0x9F)
			if( (asm -> opcode & 0xF8) == 0x90 )
				// show supplementary register (AX, EAX, RAX)
				lib_asm_register( asm, 0, asm -> rex.r << 3 );

			// mov (0xB0-0xBF)
			if( (asm -> opcode & ~STD_MASK_byte_half) == 0xB0 ) goto next;	// add immediate value

			// done
			goto end;
		} else

		// memory offset access?
		if( asm -> instruction.options & FO ) {
			// current bits
			uint8_t bits = asm -> register_bits;

			// change to 16 bit mode?
			if( asm -> register_semaphore ) bits = WORD;

			// change to 64 bit mode?
			if( ! asm -> register_semaphore && asm -> rex.w ) bits = QWORD;

			// strictly definied size for operand?
			if( asm -> instruction.options & B ) bits = BYTE;
			if( asm -> instruction.options & W ) bits = WORD;
			if( asm -> instruction.options & D ) bits = DWORD;

			// first operand is value?
			if( asm -> instruction.options & V ) {
				// show memory access
				log( LIB_ASM_COLOR_MEMORY"%s ["LIB_ASM_COLOR_IMMEDIATE"0x%16X"LIB_ASM_COLOR_MEMORY"],"LIB_ASM_SEPARATOR, lib_asm_size[ bits ], (uint64_t) *((uint64_t *) asm -> rip) );

				// and register
				lib_asm_register( asm, 0, asm -> rex.r << 3 );
			// second is
			} else {
				// show
				lib_asm_register( asm, 0, asm -> rex.r << 3 );

				// show memory access
				log( ","LIB_ASM_SEPARATOR""LIB_ASM_COLOR_MEMORY"%s ["LIB_ASM_COLOR_IMMEDIATE"0x%16X"LIB_ASM_COLOR_MEMORY"]", lib_asm_size[ bits ], (uint64_t) *((uint64_t *) asm -> rip) );
			}

			// leave value
			asm -> rip += 8;

			// done
			goto end;
		} else

		// no register at all
		if( asm -> instruction.options & FE ) goto next;	// yeah

		// only Accumulator register (AL, AX, EAX, RAX)
		else lib_asm_register( asm, 0, asm -> rex.r << 3 );
	}

next:
	// check for any left operand, which can be immediate
	lib_asm_value( asm );

end:
	// amount of parsed Bytes
	return (uintptr_t) asm -> rip - (uintptr_t) rip;
}
