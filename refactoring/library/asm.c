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
		log( LIB_ASM_COLOR_INSTRUCTION"db "LIB_ASM_COLOR_DATA"0x%2X", asm -> opcode );

		// we are done
		goto end;
	}

	// instruction name
	if( ! lib_asm_name( asm ) ) goto end;
	
	// only instruction name?
	if( ! asm -> instruction.options ) goto end;	// yes

	// for instructions which belongs to group
	if( asm -> instruction.group ) {
		// direct register access?
		if( asm -> modrm.mod == 0x03 ) {
			// default bits
			uint8_t bits = asm -> register_bits;

			// proposed size for operand? (only, if not 16 bit already)
			if( bits != 1 ) {
				if( asm -> instruction.options & B ) bits = BYTE;
				if( asm -> instruction.options & D ) bits = DWORD;
			}

			// register size override by REX?
			if( asm -> rex.w ) bits = QWORD;	// forced 64 bit

			// show
			log( LIB_ASM_COLOR_REGISTER"%s", r[ bits ][ asm -> modrm.rm | asm -> rex.b << 3 ] );	// 0xC0 opcode required, asm -> rex.b << 3, delete comment
		// exception for opcode: 0xE3
		} else if( asm -> opcode == 0xE3 ) goto leave;	// nothing to do in here

		// show
		else lib_asm_memory( asm );

		// exception for opcode: 0xD0, 0xD1, 0xD2, 0xD3
		if( asm -> opcode == 0xD0 || asm -> opcode == 0xD1 ) { log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR""LIB_ASM_COLOR_IMMEDIATE"0x01" ); goto end; }
		if( asm -> opcode == 0xD2 || asm -> opcode == 0xD3 ) { log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR""LIB_ASM_COLOR_REGISTER"cl" ); goto end; }

		// separator required from now on
		asm -> comma_semaphore = TRUE;
	} else {
		// try to parse ModR/M, if exist
		if( ! lib_asm_modrm( asm ) ) {	// doesn't
			// offset exist?
			if( asm -> instruction.options & FT ) {
				// first operand is a register?
				if( asm -> instruction.options & R ) {
					// show
					lib_asm_register( asm, 0, asm -> modrm.reg | (asm -> rex.r << 3) );

					// show memory access
					log( ","LIB_ASM_SEPARATOR""LIB_ASM_COLOR_MEMORY"["LIB_ASM_COLOR_IMMEDIATE"0x%8X"LIB_ASM_COLOR_MEMORY"]", (uint32_t) *((uint32_t *) asm -> rip) );
				} else {
					// show memory access
					log( LIB_ASM_COLOR_MEMORY"["LIB_ASM_COLOR_IMMEDIATE"0x%8X"LIB_ASM_COLOR_MEMORY"],"LIB_ASM_SEPARATOR, (uint32_t) *((uint32_t *) asm -> rip) );

					// and register
					lib_asm_register( asm, 0, asm -> modrm.reg | (asm -> rex.r << 3) );
				}

				// leave value for memory access
				asm -> rip += 4;
			// first operand is a register?
			} else if( asm -> instruction.options & R ) {
				// show
				lib_asm_register( asm, 0, asm -> modrm.reg | (asm -> rex.r << 3) );

				// separator required from now on
				asm -> comma_semaphore = TRUE;
			} else {
				// REFACTORING REQUIRED

				// register might be a hidden inside opcode
				if( asm -> instruction.options & FH ) {
					// opcode exceptions
					switch( asm -> opcode ) {
						case 0xE4: {
							// destination register
							log( LIB_ASM_COLOR_REGISTER"al" );

							// done
							break;
						}

						case 0xE5: {
							// destination register
							log( LIB_ASM_COLOR_REGISTER"%s", r[ asm -> register_bits ][ 0 ] );

							// done
							break;
						}

						case 0xE6: {
							// destination address
							log( LIB_ASM_COLOR_IMMEDIATE"0x%2X", *(asm -> rip++) );

							// source register
							log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR""LIB_ASM_COLOR_REGISTER"al" );

							// done
							goto end;
						}

						case 0xE7: {
							// destination address
							log( LIB_ASM_COLOR_IMMEDIATE"0x%2X", *(asm -> rip++) );

							// source register
							log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR""LIB_ASM_COLOR_REGISTER"%s", r[ asm -> register_bits ][ 0 ] );

							// done
							goto end;
						}

						case 0xEC: {
							// destination address
							log( LIB_ASM_COLOR_REGISTER"al" );

							// source register
							log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR""LIB_ASM_COLOR_REGISTER"dx" );

							// done
							goto end;
						}

						case 0xED: {
							// destination address
							log( LIB_ASM_COLOR_REGISTER"%s", r[ asm -> register_bits ][ 0 ] );

							// source register
							log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR""LIB_ASM_COLOR_REGISTER"dx" );

							// done
							goto end;
						}

						case 0xEE: {
							// destination address
							log( LIB_ASM_COLOR_REGISTER"dx" );

							// source register
							log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR""LIB_ASM_COLOR_REGISTER"al" );

							// done
							goto end;
						}

						case 0xEF: {
							// destination address
							log( LIB_ASM_COLOR_REGISTER"dx" );

							// source register
							log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR""LIB_ASM_COLOR_REGISTER"%s", r[ asm -> register_bits ][ 0 ] );

							// done
							goto end;
						}

						default: {
							// push/pop?
							if( (asm -> opcode & ~STD_MASK_byte_half) == 0x50 )
								// only 64 bit registers are allowed
								asm -> register_bits = QWORD;

							// register
							uint8_t reg = asm -> opcode & STD_MASK_byte_half;

							// instruction POP, alignment
							if( reg > 0x07 ) reg -= 0x08;

							// show
							lib_asm_register( asm, 0, reg | (asm -> rex.b << 3) );

							// xchg?
							if( asm -> opcode >= 0x90 && asm -> opcode <= 0x97 )
								// show suplementary register
								lib_asm_register( asm, 0, 0 );

							// end?
							if( (asm -> opcode & ~STD_MASK_byte_half) != 0xB0 ) goto end;
						}
					}

					// separator required from now on
					asm -> comma_semaphore = TRUE;
				}
			}
		}
	}

leave:
	// check for any left operand, which can be immediate
	lib_asm_value( asm );

end:
	// amount of parsed Bytes
	return (uintptr_t) asm -> rip - (uintptr_t) rip;
}
