/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t *lib_asm_registers[ 4 ][ 16 ] = {
	{ (uint8_t *) "al", (uint8_t *) "cl", (uint8_t *) "dl", (uint8_t *) "bl", (uint8_t *) "spl", (uint8_t *) "bpl", (uint8_t *) "sil", (uint8_t *) "dil", (uint8_t *) "r8b", (uint8_t *) "r9b", (uint8_t *) "r10b", (uint8_t *) "r11b", (uint8_t *) "r12b", (uint8_t *) "r13b", (uint8_t *) "r14b", (uint8_t *) "r15b" },
	{ (uint8_t *) "ax", (uint8_t *) "cx", (uint8_t *) "dx", (uint8_t *) "bx", (uint8_t *) "sp", (uint8_t *) "bp", (uint8_t *) "si", (uint8_t *) "di", (uint8_t *) "r8w", (uint8_t *) "r9w", (uint8_t *) "r10w", (uint8_t *) "r11w", (uint8_t *) "r12w", (uint8_t *) "r13w", (uint8_t *) "r14w", (uint8_t *) "r15w" },
	{ (uint8_t *) "eax", (uint8_t *) "ecx", (uint8_t *) "edx", (uint8_t *) "ebx", (uint8_t *) "esp", (uint8_t *) "ebp", (uint8_t *) "esi", (uint8_t *) "edi", (uint8_t *) "r8d", (uint8_t *) "r9d", (uint8_t *) "r10d", (uint8_t *) "r11d", (uint8_t *) "r12d", (uint8_t *) "r13d", (uint8_t *) "r14d", (uint8_t *) "r15d" },
	{ (uint8_t *) "rax", (uint8_t *) "rcx", (uint8_t *) "rdx", (uint8_t *) "rbx", (uint8_t *) "rsp", (uint8_t *) "rbp", (uint8_t *) "rsi", (uint8_t *) "rdi", (uint8_t *) "r8", (uint8_t *) "r9", (uint8_t *) "r10", (uint8_t *) "r11", (uint8_t *) "r12", (uint8_t *) "r13", (uint8_t *) "r14", (uint8_t *) "r15" }
};

uint8_t *lib_asm_registers_additional[] = {
	(uint8_t *) "al", (uint8_t *) "cl", (uint8_t *) "dl", (uint8_t *) "bl", (uint8_t *) "ah", (uint8_t *) "ch", (uint8_t *) "dh", (uint8_t *) "bh"
};

uint8_t *lib_asm_scale[] = { (uint8_t *) "", (uint8_t *) "2", (uint8_t *) "4", (uint8_t *) "8" };

uint8_t *lib_asm_size[] = { (uint8_t *) "byte", (uint8_t *) "word", (uint8_t *) "dword", (uint8_t *) "qword" };

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_nop[ 1 ] = {
	{ EMPTY }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_pause[ 1 ] = {
	{ (uint8_t *) "pause" }
};

// struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_1[] = {
// 	{ (uint8_t *) "add" },
// 	{ (uint8_t *) "or"  },
// 	{ (uint8_t *) "adc" },
// 	{ (uint8_t *) "sbb" },
// 	{ (uint8_t *) "and" },
// 	{ (uint8_t *) "sub" },
// 	{ (uint8_t *) "xor" },
// 	{ (uint8_t *) "cmp" }
// };

// struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_2[] = {
// 	{ (uint8_t *) "rol" },
// 	{ (uint8_t *) "ror" },
// 	{ (uint8_t *) "rcl" },
// 	{ (uint8_t *) "rcr" },
// 	{ (uint8_t *) "shl" },
// 	{ (uint8_t *) "shr" },
// 	{ (uint8_t *) "" },
// 	{ (uint8_t *) "sar" }
// };

// struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_3[] = {
// 	{ (uint8_t *) "mov" }
// };

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_a[] = {
	{ EMPTY },
	{ (uint8_t *) "insw" },
	{ (uint8_t *) "insd" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_b[] = {
	{ EMPTY },
	{ (uint8_t *) "outsw" },
	{ (uint8_t *) "outsd" }
};

// struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_a[] = {
// 	{ EMPTY },
// 	{ (uint8_t *) "cbw" },
// 	{ (uint8_t *) "cwde" },
// 	{ (uint8_t *) "cdqe" }
// };

// struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_b[] = {
// 	{ EMPTY },
// 	{ (uint8_t *) "cwd" },
// 	{ (uint8_t *) "cdq" },
// 	{ (uint8_t *) "cqo" }
// };

// struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_c[] = {
// 	{ (uint8_t *) "movsb" },
// 	{ (uint8_t *) "movsw" },
// 	{ (uint8_t *) "movsd" },
// 	{ (uint8_t *) "movsq" }
// };

// struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_d[] = {
// 	{ (uint8_t *) "cmpsb" },
// 	{ (uint8_t *) "cmpsw" },
// 	{ (uint8_t *) "cmpsd" },
// 	{ (uint8_t *) "cmpsq" }
// };

// struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_e[] = {
// 	{ (uint8_t *) "stosb" },
// 	{ (uint8_t *) "stosw" },
// 	{ (uint8_t *) "stosd" },
// 	{ (uint8_t *) "stosq" }
// };

// struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_f[] = {
// 	{ (uint8_t *) "lodsb" },
// 	{ (uint8_t *) "lodsw" },
// 	{ (uint8_t *) "lodsd" },
// 	{ (uint8_t *) "lodsq" }
// };

// struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_g[] = {
// 	{ (uint8_t *) "scasb" },
// 	{ (uint8_t *) "scasw" },
// 	{ (uint8_t *) "scasd" },
// 	{ (uint8_t *) "scasq" }
// };

// struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_h[] = {
// 	{ EMPTY },
// 	{ EMPTY },
// 	{ (uint8_t *) "jecxz" },
// 	{ (uint8_t *) "jrcxz" }
// };

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_master[] = {
	{ (uint8_t *) "add", (M|B) | (B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 | FV },
	{ (uint8_t *) "add", (M) | FM | FV },
	{ (uint8_t *) "add", (B) | (M|B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 },
	{ (uint8_t *) "add", (M) << LIB_ASM_OPTION_OPERAND_offset | FM },
	{ (uint8_t *) "add", (B) | (V|B) << LIB_ASM_OPTION_OPERAND_offset | F0 | F1 },
	{ (uint8_t *) "add", (V) << LIB_ASM_OPTION_OPERAND_offset },
	{ EMPTY },
	{ EMPTY },
	{ (uint8_t *) "or", (M|B) | (B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 | FV },
	{ (uint8_t *) "or", (M) | FM | FV },
	{ (uint8_t *) "or", (B) | (M|B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 },
	{ (uint8_t *) "or", (M) << LIB_ASM_OPTION_OPERAND_offset | FM },
	{ (uint8_t *) "or", (B) | (V|B) << LIB_ASM_OPTION_OPERAND_offset | F0 | F1 },
	{ (uint8_t *) "or", (V) << LIB_ASM_OPTION_OPERAND_offset },
	{ EMPTY },
	{ EMPTY },
	{ (uint8_t *) "adc", (M|B) | (B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 | FV },
	{ (uint8_t *) "adc", (M) | FM | FV },
	{ (uint8_t *) "adc", (B) | (M|B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 },
	{ (uint8_t *) "adc", (M) << LIB_ASM_OPTION_OPERAND_offset | FM },
	{ (uint8_t *) "adc", (B) | (V|B) << LIB_ASM_OPTION_OPERAND_offset | F0 | F1 },
	{ (uint8_t *) "adc", (V) << LIB_ASM_OPTION_OPERAND_offset },
	{ EMPTY },
	{ EMPTY },
	{ (uint8_t *) "sbb", (M|B) | (B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 | FV },
	{ (uint8_t *) "sbb", (M) | FM | FV },
	{ (uint8_t *) "sbb", (B) | (M|B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 },
	{ (uint8_t *) "sbb", (M) << LIB_ASM_OPTION_OPERAND_offset | FM },
	{ (uint8_t *) "sbb", (B) | (V|B) << LIB_ASM_OPTION_OPERAND_offset | F0 | F1 },
	{ (uint8_t *) "sbb", (V) << LIB_ASM_OPTION_OPERAND_offset },
	{ EMPTY },
	{ EMPTY },
	{ (uint8_t *) "and", (M|B) | (B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 | FV },
	{ (uint8_t *) "and", (M) | FM | FV },
	{ (uint8_t *) "and", (B) | (M|B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 },
	{ (uint8_t *) "and", (M) << LIB_ASM_OPTION_OPERAND_offset | FM },
	{ (uint8_t *) "and", (B) | (V|B) << LIB_ASM_OPTION_OPERAND_offset | F0 | F1 },
	{ (uint8_t *) "and", (V) << LIB_ASM_OPTION_OPERAND_offset },
	{ EMPTY },
	{ EMPTY },
	{ (uint8_t *) "sub", (M|B) | (B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 | FV },
	{ (uint8_t *) "sub", (M) | FM | FV },
	{ (uint8_t *) "sub", (B) | (M|B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 },
	{ (uint8_t *) "sub", (M) << LIB_ASM_OPTION_OPERAND_offset | FM },
	{ (uint8_t *) "sub", (B) | (V|B) << LIB_ASM_OPTION_OPERAND_offset | F0 | F1 },
	{ (uint8_t *) "sub", (V) << LIB_ASM_OPTION_OPERAND_offset },
	{ EMPTY },
	{ EMPTY },
	{ (uint8_t *) "xor", (M|B) | (B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 | FV },
	{ (uint8_t *) "xor", (M) | FM | FV },
	{ (uint8_t *) "xor", (B) | (M|B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 },
	{ (uint8_t *) "xor", (M) << LIB_ASM_OPTION_OPERAND_offset | FM },
	{ (uint8_t *) "xor", (B) | (V|B) << LIB_ASM_OPTION_OPERAND_offset | F0 | F1 },
	{ (uint8_t *) "xor", (V) << LIB_ASM_OPTION_OPERAND_offset },
	{ EMPTY },
	{ EMPTY },
	{ (uint8_t *) "cmp", (M|B) | (B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 | FV },
	{ (uint8_t *) "cmp", (M) | FM | FV },
	{ (uint8_t *) "cmp", (B) | (M|B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 },
	{ (uint8_t *) "cmp", (M) << LIB_ASM_OPTION_OPERAND_offset | FM },
	{ (uint8_t *) "cmp", (B) | (V|B) << LIB_ASM_OPTION_OPERAND_offset | F0 | F1 },
	{ (uint8_t *) "cmp", (V) << LIB_ASM_OPTION_OPERAND_offset },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ (uint8_t *) "push", FR },
	{ (uint8_t *) "push", FR },
	{ (uint8_t *) "push", FR },
	{ (uint8_t *) "push", FR },
	{ (uint8_t *) "push", FR },
	{ (uint8_t *) "push", FR },
	{ (uint8_t *) "push", FR },
	{ (uint8_t *) "push", FR },
	{ (uint8_t *) "pop", FR },
	{ (uint8_t *) "pop", FR },
	{ (uint8_t *) "pop", FR },
	{ (uint8_t *) "pop", FR },
	{ (uint8_t *) "pop", FR },
	{ (uint8_t *) "pop", FR },
	{ (uint8_t *) "pop", FR },
	{ (uint8_t *) "pop", FR },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ (uint8_t *) "movsxd", (D) << LIB_ASM_OPTION_OPERAND_offset | FM | F1 },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ (uint8_t *) "push", (V) | FE },
	{ (uint8_t *) "imul", (M) << LIB_ASM_OPTION_OPERAND_offset | (V) << (LIB_ASM_OPTION_OPERAND_offset * 2) | FM },
	{ (uint8_t *) "push", (V|B) | FE | F0 },
	{ (uint8_t *) "imul", (M) << LIB_ASM_OPTION_OPERAND_offset | (V|B) << (LIB_ASM_OPTION_OPERAND_offset * 2) | FM | F2 },
	{ (uint8_t *) "insb" },
	{ (uint8_t *) "insw/d", EMPTY, lib_asm_instruction_group_a },
	{ (uint8_t *) "outsb" },
	{ (uint8_t *) "outsw/d", EMPTY, lib_asm_instruction_group_b },
	{ (uint8_t *) "jo", (V|B) | FE | FR | F0 },
	{ (uint8_t *) "jno", (V|B) | FE | FR | F0 },
	{ (uint8_t *) "jb", (V|B) | FE | FR | F0 },
	{ (uint8_t *) "jnb", (V|B) | FE | FR | F0 },
	{ (uint8_t *) "je", (V|B) | FE | FR | F0 },
	{ (uint8_t *) "jne", (V|B) | FE | FR | F0 },
	{ (uint8_t *) "jbe", (V|B) | FE | FR | F0 },
	{ (uint8_t *) "jnbe", (V|B) | FE | FR | F0 },
	{ (uint8_t *) "js", (V|B) | FE | FR | F0 },
	{ (uint8_t *) "jns", (V|B) | FE | FR | F0 },
	{ (uint8_t *) "jp", (V|B) | FE | FR | F0 },
	{ (uint8_t *) "jnp", (V|B) | FE | FR | F0 },
	{ (uint8_t *) "jl", (V|B) | FE | FR | F0 },
	{ (uint8_t *) "jnl", (V|B) | FE | FR | F0 },
	{ (uint8_t *) "jle", (V|B) | FE | FR | F0 },
	{ (uint8_t *) "jnle", (V|B) | FE | FR | F0 },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY }
};


	// { EMPTY, (M|B) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FO, group_1 },				// 0x80
	// { EMPTY, (M|D) | (I|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FO, group_1 },				// 0x81
	// { EMPTY },				// 0x82
	// { EMPTY, (M|D) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FO, group_1 },				// 0x83
	// { (uint8_t *) "test", (M|B) | ((B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FM | FO | FI },				// 0x84
	// { (uint8_t *) "test", (M|D) | (D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FO | FI },					// 0x85
	// { (uint8_t *) "xchg", (M|B) | ((B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FM | FO | FI },				// 0x86
	// { (uint8_t *) "xchg", (M|D) | (D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FO | FI },				// 0x87
	// { (uint8_t *) "mov", (M|B) | ((B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FM | FO | FI },				// 0x88
	// { (uint8_t *) "mov", (M|D) | FM | FI },				// 0x89
	// { (uint8_t *) "mov", (B) | ((M|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FM | FO | FI },				// 0x8A
	// { (uint8_t *) "mov", ((M|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FM | FI },				// 0x8B
	// { EMPTY },				// 0x8C
	// { (uint8_t *) "lea", FM },				// 0x8D
	// { EMPTY },				// 0x8E
	// { (uint8_t *) "pop", FM },				// 0x8F
	// { (uint8_t *) "xchg", FH },				// 0x90
	// { (uint8_t *) "xchg", FH },				// 0x91
	// { (uint8_t *) "xchg", FH },				// 0x92
	// { (uint8_t *) "xchg", FH },				// 0x93
	// { (uint8_t *) "xchg", FH },				// 0x94
	// { (uint8_t *) "xchg", FH },				// 0x95
	// { (uint8_t *) "xchg", FH },				// 0x96
	// { (uint8_t *) "xchg", FH },				// 0x97
	// { (uint8_t *) "cbw", EMPTY, group_a },				// 0x98
	// { (uint8_t *) "cwd", EMPTY, group_b },				// 0x99
	// { EMPTY },				// 0x9A
	// { (uint8_t *) "fwait" },				// 0x9B
	// { (uint8_t *) "pushfq" },				// 0x9C
	// { (uint8_t *) "popfq" },				// 0x9D
	// { (uint8_t *) "sahf" },				// 0x9E
	// { (uint8_t *) "lahf" },				// 0x9F
	// { (uint8_t *) "mov", B | FM | FO },				// 0xA0
	// { (uint8_t *) "mov", R | FT },				// 0xA1
	// { (uint8_t *) "mov", B | (M) | FM | FO },				// 0xA2
	// { (uint8_t *) "mov", ((R) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FT },				// 0xA3
	// { (uint8_t *) "movsb" },				// 0xA4
	// { (uint8_t *) "movsb", EMPTY, group_c },				// 0xA5
	// { (uint8_t *) "cmpsb" },				// 0xA6
	// { (uint8_t *) "cmpsb", EMPTY, group_d },				// 0xA7
	// { (uint8_t *) "test", (R|B) | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FO },		// 0xA8
	// { (uint8_t *) "test", (R) | (I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift },		// 0xA9
	// { (uint8_t *) "stosb" },				// 0xAA
	// { (uint8_t *) "stosb", EMPTY, group_e },				// 0xAB
	// { (uint8_t *) "lodsb" },				// 0xAC
	// { (uint8_t *) "lodsb", EMPTY, group_f },				// 0xAD
	// { (uint8_t *) "scasb" },				// 0xAE
	// { (uint8_t *) "scasb", EMPTY, group_g },				// 0xAF
	// { (uint8_t *) "mov", B | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH | FO },				// 0xB0
	// { (uint8_t *) "mov", B | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH | FO },				// 0xB1
	// { (uint8_t *) "mov", B | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH | FO },				// 0xB2
	// { (uint8_t *) "mov", B | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH | FO },				// 0xB3
	// { (uint8_t *) "mov", B | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH | FO },				// 0xB4
	// { (uint8_t *) "mov", B | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH | FO },				// 0xB5
	// { (uint8_t *) "mov", B | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH | FO },				// 0xB6
	// { (uint8_t *) "mov", B | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH | FO },				// 0xB7
	// { (uint8_t *) "mov", ((I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH	},			// 0xB8
	// { (uint8_t *) "mov", ((I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH	},				// 0xB9
	// { (uint8_t *) "mov", ((I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH	},				// 0xBA
	// { (uint8_t *) "mov", ((I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH	},				// 0xBB
	// { (uint8_t *) "mov", ((I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH	},				// 0xBC
	// { (uint8_t *) "mov", ((I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH	},				// 0xBD
	// { (uint8_t *) "mov", ((I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH	},				// 0xBE
	// { (uint8_t *) "mov", ((I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH	},				// 0xBF
	// { EMPTY, (M|B) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FO, group_2 },				// 0xC0
	// { EMPTY, (M|D) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM, group_2 },				// 0xC1
	// { (uint8_t *) "retn", (I|W) | FO },				// 0xC2
	// { (uint8_t *) "retn" },				// 0xC3
	// { EMPTY },				// 0xC4
	// { EMPTY },				// 0xC5
	// { EMPTY, (M|B) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FO, group_3 },				// 0xC6
	// { EMPTY, (M|D) | (I|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FO, group_3 },				// 0xC7
	// { (uint8_t *) "enter", (I|W) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FO },				// 0xC8
	// { (uint8_t *) "leave" },		// 0xC9
	// { (uint8_t *) "retf", (I|W) | FO },	// 0xCA
	// { (uint8_t *) "retf" },				// 0xCB
	// { (uint8_t *) "int" },				// 0xCC
	// { (uint8_t *) "int", (I|B) | FO },				// 0xCD
	// { (uint8_t *) "into" },				// 0xCE
	// { (uint8_t *) "iretq" },				// 0xCF
	// { EMPTY, (M|B) | FM | FO, group_2 },				// 0xD0
	// { EMPTY, (M|D) | FM, group_2 },				// 0xD1
	// { EMPTY, (M|B) | FM | FO, group_2 },				// 0xD2
	// { EMPTY, (M|D) | FM, group_2 },				// 0xD3
	// { EMPTY },				// 0xD4
	// { EMPTY },				// 0xD5
	// { EMPTY },				// 0xD6
	// { EMPTY },				// 0xD7
	// { EMPTY },				// 0xD8
	// { EMPTY },				// 0xD9
	// { EMPTY },				// 0xDA
	// { EMPTY },				// 0xDB
	// { EMPTY },				// 0xDC
	// { EMPTY },				// 0xDD
	// { EMPTY },				// 0xDE
	// { EMPTY },				// 0xDF
	// { (uint8_t *) "loopnz", (I|B) | FR | FO },				// 0xE0
	// { (uint8_t *) "loopz", (I|B) | FR | FO },				// 0xE1
	// { (uint8_t *) "loop", (I|B) | FR | FO },				// 0xE2
	// { (uint8_t *) "jcxz", (I|B) | FR | FO, group_h },				// 0xE3
	// { (uint8_t *) "in", B | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH | FO },				// 0xE4
	// { (uint8_t *) "in", ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH },				// 0xE5
	// { (uint8_t *) "out", FH },			// 0xE6
	// { (uint8_t *) "out", FH },				// 0xE7
	// { (uint8_t *) "call", (I) | FR },				// 0xE8
	// { (uint8_t *) "jmp", (I) | FR },				// 0xE9
	// { EMPTY },				// 0xEA
	// { (uint8_t *) "jmp", (I|B) | FO | FR },				// 0xEB
	// { (uint8_t *) "in", FH },				// 0xEC
	// { (uint8_t *) "in", FH },				// 0xED
	// { (uint8_t *) "out", FH },				// 0xEE
	// { (uint8_t *) "out", FH },				// 0xEF
	// { EMPTY },				// 0xF0
	// { EMPTY },				// 0xF1
	// { EMPTY },				// 0xF2
	// { EMPTY },				// 0xF3
	// { (uint8_t *) "hlt" },				// 0xF4
	// { (uint8_t *) "cmc" },				// 0xF5
	// { EMPTY },				// 0xF6
	// { EMPTY },				// 0xF7
	// { (uint8_t *) "clc" },				// 0xF8
	// { (uint8_t *) "stc" },				// 0xF9
	// { (uint8_t *) "cli" },				// 0xFA
	// { (uint8_t *) "sti" },				// 0xFB
	// { (uint8_t *) "cld" },				// 0xFC
	// { (uint8_t *) "std" },				// 0xFD
	// { EMPTY },				// 0xFE
	// { EMPTY }				// 0xFF
// };

// struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_slave[] = {
// };
