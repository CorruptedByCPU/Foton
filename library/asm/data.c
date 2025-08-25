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
	{ (uint8_t *) "nop" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_pause[ 1 ] = {
	{ (uint8_t *) "pause" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_1[] = {
	{ (uint8_t *) "add" },
	{ (uint8_t *) "or"  },
	{ (uint8_t *) "adc" },
	{ (uint8_t *) "sbb" },
	{ (uint8_t *) "and" },
	{ (uint8_t *) "sub" },
	{ (uint8_t *) "xor" },
	{ (uint8_t *) "cmp" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_2[] = {
	{ (uint8_t *) "rol" },
	{ (uint8_t *) "ror" },
	{ (uint8_t *) "rcl" },
	{ (uint8_t *) "rcr" },
	{ (uint8_t *) "shl" },
	{ (uint8_t *) "shr" },
	{ (uint8_t *) "" },
	{ (uint8_t *) "sar" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_3[] = {
	{ (uint8_t *) "mov" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_4[] = {
	{ (uint8_t *) "test", (M|B) | (V|B) << LIB_ASM_OPTION_OPERAND_offset | FM | FE | F0 | F1 },
	{ (uint8_t *) "test", (M|B) | (V|B) << LIB_ASM_OPTION_OPERAND_offset | FM | FE | F0 | F1 },
	{ (uint8_t *) "not", (M|B) | FM | FE | F0 },
	{ (uint8_t *) "neg", (M|B) | FM | FE | F0 },
	{ (uint8_t *) "mul", (M|B) | FM | FE | F0 },
	{ (uint8_t *) "imul", (M|B) | FM | FE | F0 },
	{ (uint8_t *) "div", (M|B) | FM | FE | F0 },
	{ (uint8_t *) "idiv", (M|B) | FM | FE | F0 }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_5[] = {
	{ (uint8_t *) "test", (M) | (V) << LIB_ASM_OPTION_OPERAND_offset | FM | FE },
	{ (uint8_t *) "test", (M) | (V) << LIB_ASM_OPTION_OPERAND_offset | FM | FE },
	{ (uint8_t *) "not", (M) | FM | FE | F0 },
	{ (uint8_t *) "neg", (M) | FM | FE | F0 },
	{ (uint8_t *) "mul", (M) | FM | FE | F0 },
	{ (uint8_t *) "imul", (M) | FM | FE | F0 },
	{ (uint8_t *) "div", (M) | FM | FE | F0 },
	{ (uint8_t *) "idiv", (M) | FM | FE | F0 }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_6[] = {
	{ (uint8_t *) "inc" },
	{ (uint8_t *) "dec" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_7[] = {
	{ (uint8_t *) "inc", (M) | FM | FE },
	{ (uint8_t *) "dec", (M) | FM | FE },
	{ (uint8_t *) "call", (M) | FM | FE },
	{ (uint8_t *) "callf", (M) | FM | FE },
	{ (uint8_t *) "jmp", (M) | FM | FE },
	{ (uint8_t *) "jmpf", (M) | FM | FE },
	{ (uint8_t *) "push", (M) | FM | FE },
};

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

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_c[] = {
	{ EMPTY },
	{ (uint8_t *) "cbw" },
	{ (uint8_t *) "cwde" },
	{ (uint8_t *) "cdqe" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_d[] = {
	{ EMPTY },
	{ (uint8_t *) "cwd" },
	{ (uint8_t *) "cdq" },
	{ (uint8_t *) "cqo" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_e[] = {
	{ (uint8_t *) "movsb" },
	{ (uint8_t *) "movsw" },
	{ (uint8_t *) "movsd" },
	{ (uint8_t *) "movsq" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_f[] = {
	{ (uint8_t *) "cmpsb" },
	{ (uint8_t *) "cmpsw" },
	{ (uint8_t *) "cmpsd" },
	{ (uint8_t *) "cmpsq" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_g[] = {
	{ (uint8_t *) "stosb" },
	{ (uint8_t *) "stosw" },
	{ (uint8_t *) "stosd" },
	{ (uint8_t *) "stosq" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_h[] = {
	{ (uint8_t *) "lodsb" },
	{ (uint8_t *) "lodsw" },
	{ (uint8_t *) "lodsd" },
	{ (uint8_t *) "lodsq" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_group_i[] = {
	{ (uint8_t *) "scasb" },
	{ (uint8_t *) "scasw" },
	{ (uint8_t *) "scasd" },
	{ (uint8_t *) "scasq" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_master[] = {
	// 0x00
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
	// 0x10
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
	// 0x20
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
	// 0x30
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
	// 0x40
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
	// 0x50
	{ (uint8_t *) "push", FD },
	{ (uint8_t *) "push", FD },
	{ (uint8_t *) "push", FD },
	{ (uint8_t *) "push", FD },
	{ (uint8_t *) "push", FD },
	{ (uint8_t *) "push", FD },
	{ (uint8_t *) "push", FD },
	{ (uint8_t *) "push", FD },
	{ (uint8_t *) "pop", FD },
	{ (uint8_t *) "pop", FD },
	{ (uint8_t *) "pop", FD },
	{ (uint8_t *) "pop", FD },
	{ (uint8_t *) "pop", FD },
	{ (uint8_t *) "pop", FD },
	{ (uint8_t *) "pop", FD },
	{ (uint8_t *) "pop", FD },
	// 0x60
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
	// 0x70
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
	// 0x80
	{ EMPTY, (M|B) | (V|B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1, lib_asm_instruction_group_1 },
	{ EMPTY, (M) | (V) << LIB_ASM_OPTION_OPERAND_offset | FM, lib_asm_instruction_group_1 },
	{ EMPTY },
	{ EMPTY, (M) | (V|B) << LIB_ASM_OPTION_OPERAND_offset | FM | F1, lib_asm_instruction_group_1 },	// removed signed flag
	{ (uint8_t *) "test", (M|B) | (B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 | FV },
	{ (uint8_t *) "test", (M) | FM | FV },
	{ (uint8_t *) "xchg", (B) | (M|B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 | FV },
	{ (uint8_t *) "xchg", (M) | FM | FV },
	{ (uint8_t *) "mov", (M|B) | (B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 | FV },
	{ (uint8_t *) "mov", (M) | FM | FV },
	{ (uint8_t *) "mov", (B) | (M|B) << LIB_ASM_OPTION_OPERAND_offset | FM | F0 | F1 | FV },
	{ (uint8_t *) "mov", (M) << LIB_ASM_OPTION_OPERAND_offset | FM | FV },
	{ EMPTY },
	{ (uint8_t *) "lea", FM },
	{ EMPTY },
	{ (uint8_t *) "pop", FM | FE },
	// 0x90
	{ (uint8_t *) "xchg", FD },
	{ (uint8_t *) "xchg", FD },
	{ (uint8_t *) "xchg", FD },
	{ (uint8_t *) "xchg", FD },
	{ (uint8_t *) "xchg", FD },
	{ (uint8_t *) "xchg", FD },
	{ (uint8_t *) "xchg", FD },
	{ (uint8_t *) "xchg", FD },
	{ (uint8_t *) "cbw", EMPTY, lib_asm_instruction_group_c },
	{ (uint8_t *) "cwd", EMPTY, lib_asm_instruction_group_d },
	{ EMPTY },
	{ (uint8_t *) "fwait" },
	{ (uint8_t *) "pushfq" },
	{ (uint8_t *) "popfq" },
	{ (uint8_t *) "sahf" },
	{ (uint8_t *) "lahf" },
	// 0xA0
	{ (uint8_t *) "mov", (B) | (V|B) << LIB_ASM_OPTION_OPERAND_offset | FO | F0 | F1 },
	{ (uint8_t *) "mov", (V) << LIB_ASM_OPTION_OPERAND_offset | FO },
	{ (uint8_t *) "mov", (V|B) | (B) << LIB_ASM_OPTION_OPERAND_offset | FO | F0 | F1 },
	{ (uint8_t *) "mov", (V) | FO },
	{ (uint8_t *) "movsb" },
	{ (uint8_t *) "movsb", EMPTY, lib_asm_instruction_group_e },
	{ (uint8_t *) "cmpsb" },
	{ (uint8_t *) "cmpsb", EMPTY, lib_asm_instruction_group_f },
	{ (uint8_t *) "test", (B) | (V|B) << LIB_ASM_OPTION_OPERAND_offset | F0 | F1 },
	{ (uint8_t *) "test", (V) << LIB_ASM_OPTION_OPERAND_offset },
	{ (uint8_t *) "stosb" },
	{ (uint8_t *) "stosb", EMPTY, lib_asm_instruction_group_g },
	{ (uint8_t *) "lodsb" },
	{ (uint8_t *) "lodsb", EMPTY, lib_asm_instruction_group_h },
	{ (uint8_t *) "scasb" },
	{ (uint8_t *) "scasb", EMPTY, lib_asm_instruction_group_i },
	// 0xB0
	{ (uint8_t *) "mov", B | (V|B) << LIB_ASM_OPTION_OPERAND_offset | FD | F0 | F1 },
	{ (uint8_t *) "mov", B | (V|B) << LIB_ASM_OPTION_OPERAND_offset | FD | F0 | F1 },
	{ (uint8_t *) "mov", B | (V|B) << LIB_ASM_OPTION_OPERAND_offset | FD | F0 | F1 },
	{ (uint8_t *) "mov", B | (V|B) << LIB_ASM_OPTION_OPERAND_offset | FD | F0 | F1 },
	{ (uint8_t *) "mov", B | (V|B) << LIB_ASM_OPTION_OPERAND_offset | FD | F0 | F1 },
	{ (uint8_t *) "mov", B | (V|B) << LIB_ASM_OPTION_OPERAND_offset | FD | F0 | F1 },
	{ (uint8_t *) "mov", B | (V|B) << LIB_ASM_OPTION_OPERAND_offset | FD | F0 | F1 },
	{ (uint8_t *) "mov", B | (V|B) << LIB_ASM_OPTION_OPERAND_offset | FD | F0 | F1 },
	{ (uint8_t *) "mov", (V) << LIB_ASM_OPTION_OPERAND_offset | FD },
	{ (uint8_t *) "mov", (V) << LIB_ASM_OPTION_OPERAND_offset | FD },
	{ (uint8_t *) "mov", (V) << LIB_ASM_OPTION_OPERAND_offset | FD },
	{ (uint8_t *) "mov", (V) << LIB_ASM_OPTION_OPERAND_offset | FD },
	{ (uint8_t *) "mov", (V) << LIB_ASM_OPTION_OPERAND_offset | FD },
	{ (uint8_t *) "mov", (V) << LIB_ASM_OPTION_OPERAND_offset | FD },
	{ (uint8_t *) "mov", (V) << LIB_ASM_OPTION_OPERAND_offset | FD },
	{ (uint8_t *) "mov", (V) << LIB_ASM_OPTION_OPERAND_offset | FD },
	// 0xC0
	{ EMPTY, (M|B) | (V|B) << LIB_ASM_OPTION_OPERAND_offset | FM | FV | F0 | F1, lib_asm_instruction_group_2 },
	{ EMPTY, (M) | (V|B) << LIB_ASM_OPTION_OPERAND_offset | FM | FV | F1, lib_asm_instruction_group_2 },
	{ (uint8_t *) "retn", (V|W) | FE | F0 },
	{ (uint8_t *) "retn" },
	{ EMPTY },
	{ EMPTY },
	{ EMPTY, (M|B) | (V|B) << LIB_ASM_OPTION_OPERAND_offset | FM | FV | F0 | F1, lib_asm_instruction_group_3 },
	{ EMPTY, (M) | (V) << LIB_ASM_OPTION_OPERAND_offset | FM | FV, lib_asm_instruction_group_3 },
	{ (uint8_t *) "enter", (V|W) | (V|B) << LIB_ASM_OPTION_OPERAND_offset | FE | F0 | F1 },	
	{ (uint8_t *) "leave" },
	{ (uint8_t *) "retf", (V|W) | FE | F0 },
	{ (uint8_t *) "retf" },
	{ (uint8_t *) "int3" },
	{ (uint8_t *) "int", (V|B) | F0 | FE },
	{ (uint8_t *) "into" },
	{ (uint8_t *) "iretq" },
	// 0xD0
	{ EMPTY, (M|B) | FM | FV | F0, lib_asm_instruction_group_2 },
	{ EMPTY, (M) | FM | FV, lib_asm_instruction_group_2 },
	{ EMPTY, (M|B) | FM | FV | F0, lib_asm_instruction_group_2 },
	{ EMPTY, (M) | FM | FV, lib_asm_instruction_group_2 },
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
	// 0xE0
	{ (uint8_t *) "loopnz", (V|B) | FR | F0 | FE },
	{ (uint8_t *) "loopz", (V|B) | FR | F0 | FE },
	{ (uint8_t *) "loop", (V|B) | FR | F0 | FE },
	{ (uint8_t *) "jrcxz", (V|B) | FR | F0 | FE },
	{ (uint8_t *) "in", B | (V|B) << LIB_ASM_OPTION_OPERAND_offset | F0 },
	{ (uint8_t *) "in", (V|B) << LIB_ASM_OPTION_OPERAND_offset | F0 },
	{ (uint8_t *) "out", (V|B) | FD | F0 },
	{ (uint8_t *) "out", (V|B) | FD | F0 },
	{ (uint8_t *) "call", (V) | FR | FE },
	{ (uint8_t *) "jmp", (V) | FR | FE },
	{ EMPTY },
	{ (uint8_t *) "jmp", (V|B) | F0 | FR | FE },
	{ (uint8_t *) "in", FD },
	{ (uint8_t *) "in", FD },
	{ (uint8_t *) "out", FD },
	{ (uint8_t *) "out", FD },
	// 0xF0
	{ EMPTY },
	{ (uint8_t *) "debug" },
	{ EMPTY },
	{ EMPTY },
	{ (uint8_t *) "hlt" },
	{ (uint8_t *) "cmc" },
	{ (uint8_t *) "", EMPTY, lib_asm_instruction_group_4 },
	{ (uint8_t *) "", EMPTY, lib_asm_instruction_group_5 },
	{ (uint8_t *) "clc" },
	{ (uint8_t *) "stc" },
	{ (uint8_t *) "cli" },
	{ (uint8_t *) "sti" },
	{ (uint8_t *) "cld" },
	{ (uint8_t *) "std" },
	{ EMPTY, (M|B) | FM | F0 | FV, lib_asm_instruction_group_6 },
	{ (uint8_t *) "", EMPTY, lib_asm_instruction_group_7 }
};

// struct LIB_ASM_STRUCTURE_INSTRUCTION lib_asm_instruction_slave[] = {
// };
