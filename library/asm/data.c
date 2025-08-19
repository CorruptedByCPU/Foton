/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t *r[ 4 ][ 16 ] = {
	{ (uint8_t *) "al", (uint8_t *) "cl", (uint8_t *) "dl", (uint8_t *) "bl", (uint8_t *) "spl", (uint8_t *) "bpl", (uint8_t *) "sil", (uint8_t *) "dil", (uint8_t *) "r8b", (uint8_t *) "r9b", (uint8_t *) "r10b", (uint8_t *) "r11b", (uint8_t *) "r12b", (uint8_t *) "r13b", (uint8_t *) "r14b", (uint8_t *) "r15b" },
	{ (uint8_t *) "ax", (uint8_t *) "cx", (uint8_t *) "dx", (uint8_t *) "bx", (uint8_t *) "sp", (uint8_t *) "bp", (uint8_t *) "si", (uint8_t *) "di", (uint8_t *) "r8w", (uint8_t *) "r9w", (uint8_t *) "r10w", (uint8_t *) "r11w", (uint8_t *) "r12w", (uint8_t *) "r13w", (uint8_t *) "r14w", (uint8_t *) "r15w" },
	{ (uint8_t *) "eax", (uint8_t *) "ecx", (uint8_t *) "edx", (uint8_t *) "ebx", (uint8_t *) "esp", (uint8_t *) "ebp", (uint8_t *) "esi", (uint8_t *) "edi", (uint8_t *) "r8d", (uint8_t *) "r9d", (uint8_t *) "r10d", (uint8_t *) "r11d", (uint8_t *) "r12d", (uint8_t *) "r13d", (uint8_t *) "r14d", (uint8_t *) "r15d" },
	{ (uint8_t *) "rax", (uint8_t *) "rcx", (uint8_t *) "rdx", (uint8_t *) "rbx", (uint8_t *) "rsp", (uint8_t *) "rbp", (uint8_t *) "rsi", (uint8_t *) "rdi", (uint8_t *) "r8", (uint8_t *) "r9", (uint8_t *) "r10", (uint8_t *) "r11", (uint8_t *) "r12", (uint8_t *) "r13", (uint8_t *) "r14", (uint8_t *) "r15" }
};

uint8_t *r_no_rex[] = {
	(uint8_t *) "al", (uint8_t *) "cl", (uint8_t *) "dl", (uint8_t *) "bl", (uint8_t *) "ah", (uint8_t *) "ch", (uint8_t *) "dh", (uint8_t *) "bh"
};

uint8_t *s[] = { (uint8_t *) "", (uint8_t *) "2", (uint8_t *) "4", (uint8_t *) "8" };

uint8_t *size[] = { (uint8_t *) "byte", (uint8_t *) "word", (uint8_t *) "dword", (uint8_t *) "qword" };

struct LIB_ASM_STRUCTURE_INSTRUCTION nop[ 1 ] = {
	{ EMPTY }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION pause[ 1 ] = {
	{ (uint8_t *) "pause" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION group_1[] = {
	{ (uint8_t *) "add" },
	{ (uint8_t *) "or"  },
	{ (uint8_t *) "adc" },
	{ (uint8_t *) "sbb" },
	{ (uint8_t *) "and" },
	{ (uint8_t *) "sub" },
	{ (uint8_t *) "xor" },
	{ (uint8_t *) "cmp" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION group_2[] = {
	{ (uint8_t *) "rol" },
	{ (uint8_t *) "ror" },
	{ (uint8_t *) "rcl" },
	{ (uint8_t *) "rcr" },
	{ (uint8_t *) "shl" },
	{ (uint8_t *) "shr" },
	{ (uint8_t *) "" },
	{ (uint8_t *) "sar" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION group_3[] = {
	{ (uint8_t *) "mov" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION group_a[] = {
	{ EMPTY },
	{ (uint8_t *) "cbw" },
	{ (uint8_t *) "cwde" },
	{ (uint8_t *) "cdqe" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION group_b[] = {
	{ EMPTY },
	{ (uint8_t *) "cwd" },
	{ (uint8_t *) "cdq" },
	{ (uint8_t *) "cqo" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION group_c[] = {
	{ (uint8_t *) "movsb" },
	{ (uint8_t *) "movsw" },
	{ (uint8_t *) "movsd" },
	{ (uint8_t *) "movsq" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION group_d[] = {
	{ (uint8_t *) "cmpsb" },
	{ (uint8_t *) "cmpsw" },
	{ (uint8_t *) "cmpsd" },
	{ (uint8_t *) "cmpsq" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION group_e[] = {
	{ (uint8_t *) "stosb" },
	{ (uint8_t *) "stosw" },
	{ (uint8_t *) "stosd" },
	{ (uint8_t *) "stosq" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION group_f[] = {
	{ (uint8_t *) "lodsb" },
	{ (uint8_t *) "lodsw" },
	{ (uint8_t *) "lodsd" },
	{ (uint8_t *) "lodsq" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION group_g[] = {
	{ (uint8_t *) "scasb" },
	{ (uint8_t *) "scasw" },
	{ (uint8_t *) "scasd" },
	{ (uint8_t *) "scasq" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION group_h[] = {
	{ EMPTY },
	{ EMPTY },
	{ (uint8_t *) "jecxz" },
	{ (uint8_t *) "jrcxz" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION i[] = {
	{ (uint8_t *) "add", (M|B) | ((B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FM | FO | FI },	// 0x00
	{ (uint8_t *) "add", (M) | FM | FI },	// 0x01
	{ (uint8_t *) "add", (B) | FM | FO },	// 0x02
	{ (uint8_t *) "add", (M) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM },	// 0x03
	{ (uint8_t *) "add", (R|B) | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FO },		// 0x04
	{ (uint8_t *) "add", (R) | (I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift },		// 0x05
	{ EMPTY },				// 0x06
	{ EMPTY },				// 0x07
	{ (uint8_t *) "or", (M|B) | ((B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FM | FO | FI },	// 0x08
	{ (uint8_t *) "or", (M) | FM | FI },	// 0x09
	{ (uint8_t *) "or", (B) | FM | FO },	// 0x0A
	{ (uint8_t *) "or", (M) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM },	// 0x0B
	{ (uint8_t *) "or", (R|B) | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FO },		// 0x0C
	{ (uint8_t *) "or", (R) | (I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift },		// 0x0D
	{ EMPTY },				// 0x0E
	{ EMPTY },				// 0x0F
	{ (uint8_t *) "adc", (M|B) | ((B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FM | FO | FI },	// 0x10
	{ (uint8_t *) "adc", (M) | FM | FI },	// 0x11
	{ (uint8_t *) "adc", (B) | FM | FO },	// 0x12
	{ (uint8_t *) "adc", (M) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM },	// 0x13
	{ (uint8_t *) "adc", (R|B) | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FO },		// 0x14
	{ (uint8_t *) "adc", (R) | (I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift },		// 0x15
	{ EMPTY },				// 0x16
	{ EMPTY },				// 0x17
	{ (uint8_t *) "sbb", (M|B) | ((B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FM | FO | FI },	// 0x18
	{ (uint8_t *) "sbb", (M) | FM | FI },	// 0x19
	{ (uint8_t *) "sbb", (B) | FM | FO },	// 0x1A
	{ (uint8_t *) "sbb", (M) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM },	// 0x1B
	{ (uint8_t *) "sbb", (R|B) | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FO },		// 0x1C
	{ (uint8_t *) "sbb", (R) | (I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift },		// 0x1D
	{ EMPTY },				// 0x1E
	{ EMPTY },				// 0x1F
	{ (uint8_t *) "and", (M|B) | ((B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FM | FO | FI },	// 0x20
	{ (uint8_t *) "and", (M) | FM | FI },	// 0x21
	{ (uint8_t *) "and", (B) | FM | FO },	// 0x22
	{ (uint8_t *) "and", (M) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM },	// 0x23
	{ (uint8_t *) "and", (R|B) | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FO },		// 0x24
	{ (uint8_t *) "and", (R) | (I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift },		// 0x25
	{ EMPTY },				// 0x26
	{ EMPTY },				// 0x27
	{ (uint8_t *) "sub", (M|B) | ((B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FM | FO | FI },	// 0x28
	{ (uint8_t *) "sub", (M) | FM | FI },	// 0x29
	{ (uint8_t *) "sub", (B) | FM | FO },	// 0x2A
	{ (uint8_t *) "sub", (M) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM },	// 0x2B
	{ (uint8_t *) "sub", (R|B) | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FO },		// 0x2C
	{ (uint8_t *) "sub", (R) | (I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift },		// 0x2D
	{ EMPTY },				// 0x2E
	{ EMPTY },				// 0x2F
	{ (uint8_t *) "xor", (M|B) | ((B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FM | FO | FI },	// 0x30
	{ (uint8_t *) "xor", (M) | FM | FI },	// 0x31
	{ (uint8_t *) "xor", (B) | FM | FO },	// 0x32
	{ (uint8_t *) "xor", (M) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM },	// 0x33
	{ (uint8_t *) "xor", (R|B) | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FO },		// 0x34
	{ (uint8_t *) "xor", (R) | (I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift },		// 0x35
	{ EMPTY },				// 0x36
	{ EMPTY },				// 0x37
	{ (uint8_t *) "cmp", (M|B) | ((B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FM | FO | FI },	// 0x38
	{ (uint8_t *) "cmp", (M) | FM | FI },	// 0x39
	{ (uint8_t *) "cmp", (B) | FM | FO },	// 0x3A
	{ (uint8_t *) "cmp", (M) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM },	// 0x3B
	{ (uint8_t *) "cmp", (R|B) | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FO },		// 0x3C
	{ (uint8_t *) "cmp", (R) | (I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift },		// 0x3D
	{ EMPTY },				// 0x3E
	{ EMPTY },				// 0x3F
	{ EMPTY },				// 0x40
	{ EMPTY },				// 0x41
	{ EMPTY },				// 0x42
	{ EMPTY },				// 0x43
	{ EMPTY },				// 0x44
	{ EMPTY },				// 0x45
	{ EMPTY },				// 0x46
	{ EMPTY },				// 0x47
	{ EMPTY },				// 0x48
	{ EMPTY },				// 0x49
	{ EMPTY },				// 0x4A
	{ EMPTY },				// 0x4B
	{ EMPTY },				// 0x4C
	{ EMPTY },				// 0x4D
	{ EMPTY },				// 0x4E
	{ EMPTY },				// 0x4F
	{ (uint8_t *) "push", FH },			// 0x50
	{ (uint8_t *) "push", FH },			// 0x51
	{ (uint8_t *) "push", FH },			// 0x52
	{ (uint8_t *) "push", FH },			// 0x53
	{ (uint8_t *) "push", FH },			// 0x54
	{ (uint8_t *) "push", FH },			// 0x55
	{ (uint8_t *) "push", FH },			// 0x56
	{ (uint8_t *) "push", FH },			// 0x57
	{ (uint8_t *) "pop", FH },			// 0x58
	{ (uint8_t *) "pop", FH },			// 0x59
	{ (uint8_t *) "pop", FH },			// 0x5A
	{ (uint8_t *) "pop", FH },			// 0x5B
	{ (uint8_t *) "pop", FH },			// 0x5C
	{ (uint8_t *) "pop", FH },			// 0x5D
	{ (uint8_t *) "pop", FH },			// 0x5E
	{ (uint8_t *) "pop", FH },			// 0x5F
	{ EMPTY },				// 0X60
	{ EMPTY },				// 0X61
	{ EMPTY },				// 0X62
	{ (uint8_t *) "movsxd", ((D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FM | FO },	// 0x63
	{ EMPTY },				// 0x64
	{ EMPTY },				// 0x65
	{ EMPTY },				// 0x66
	{ EMPTY },				// 0x67
	{ (uint8_t *) "push", (I) },					// 0x68
	{ (uint8_t *) "imul", (M) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | (I) << LIB_ASM_OPTION_FLAG_3rd_operand_shift | FM },	// 0x69
	{ (uint8_t *) "push", (I|B) | FO },			// 0x6A
	{ (uint8_t *) "imul", (M) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | (I|B) << LIB_ASM_OPTION_FLAG_3rd_operand_shift | FM | FO },	// 0x6B
	{ (uint8_t *) "insb" },				// 0x6C
	{ (uint8_t *) "insw" },				// 0x6D
	{ (uint8_t *) "outsb" },				// 0x6E
	{ (uint8_t *) "outsw" },				// 0x6F
	{ (uint8_t *) "jo", (I|B) | FR | FO, EMPTY },				// 0x70
	{ (uint8_t *) "jno", (I|B) | FR | FO, EMPTY },				// 0x71
	{ (uint8_t *) "jb", (I|B) | FR | FO, EMPTY },				// 0x72
	{ (uint8_t *) "jnb", (I|B) | FR | FO, EMPTY },				// 0x73
	{ (uint8_t *) "je", (I|B) | FR | FO, EMPTY },				// 0x74
	{ (uint8_t *) "jne", (I|B) | FR | FO, EMPTY },				// 0x75
	{ (uint8_t *) "jbe", (I|B) | FR | FO, EMPTY },				// 0x76
	{ (uint8_t *) "jnbe", (I|B) | FR | FO, EMPTY },				// 0x77
	{ (uint8_t *) "js", (I|B) | FR | FO, EMPTY },				// 0x78
	{ (uint8_t *) "jns", (I|B) | FR | FO, EMPTY },				// 0x79
	{ (uint8_t *) "jp", (I|B) | FR | FO, EMPTY },				// 0x7A
	{ (uint8_t *) "jnp", (I|B) | FR | FO, EMPTY },				// 0x7B
	{ (uint8_t *) "jl", (I|B) | FR | FO, EMPTY },				// 0x7C
	{ (uint8_t *) "jnl", (I|B) | FR | FO, EMPTY },				// 0x7D
	{ (uint8_t *) "jle", (I|B) | FR | FO, EMPTY },				// 0x7E
	{ (uint8_t *) "jnle", (I|B) | FR | FO, EMPTY },				// 0x7F
	{ EMPTY, (M|B) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FO, group_1 },				// 0x80
	{ EMPTY, (M|D) | (I|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FO, group_1 },				// 0x81
	{ EMPTY },				// 0x82
	{ EMPTY, (M|D) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FO, group_1 },				// 0x83
	{ (uint8_t *) "test", (M|B) | ((B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FM | FO | FI },				// 0x84
	{ (uint8_t *) "test", (M|D) | (D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FO | FI },					// 0x85
	{ (uint8_t *) "xchg", (M|B) | ((B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FM | FO | FI },				// 0x86
	{ (uint8_t *) "xchg", (M|D) | (D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FO | FI },				// 0x87
	{ (uint8_t *) "mov", (M|B) | ((B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FM | FO | FI },				// 0x88
	{ (uint8_t *) "mov", (M|D) | FM | FI },				// 0x89
	{ (uint8_t *) "mov", (B) | ((M|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FM | FO | FI },				// 0x8A
	{ (uint8_t *) "mov", ((M|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FM | FI },				// 0x8B
	{ EMPTY },				// 0x8C
	{ (uint8_t *) "lea", FM },				// 0x8D
	{ EMPTY },				// 0x8E
	{ (uint8_t *) "pop", FM },				// 0x8F
	{ (uint8_t *) "xchg", FH },				// 0x90
	{ (uint8_t *) "xchg", FH },				// 0x91
	{ (uint8_t *) "xchg", FH },				// 0x92
	{ (uint8_t *) "xchg", FH },				// 0x93
	{ (uint8_t *) "xchg", FH },				// 0x94
	{ (uint8_t *) "xchg", FH },				// 0x95
	{ (uint8_t *) "xchg", FH },				// 0x96
	{ (uint8_t *) "xchg", FH },				// 0x97
	{ (uint8_t *) "cbw", EMPTY, group_a },				// 0x98
	{ (uint8_t *) "cwd", EMPTY, group_b },				// 0x99
	{ EMPTY },				// 0x9A
	{ (uint8_t *) "fwait" },				// 0x9B
	{ (uint8_t *) "pushfq" },				// 0x9C
	{ (uint8_t *) "popfq" },				// 0x9D
	{ (uint8_t *) "sahf" },				// 0x9E
	{ (uint8_t *) "lahf" },				// 0x9F
	{ (uint8_t *) "mov", B | FM | FO },				// 0xA0
	{ (uint8_t *) "mov", R | FT },				// 0xA1
	{ (uint8_t *) "mov", B | (M) | FM | FO },				// 0xA2
	{ (uint8_t *) "mov", ((R) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FT },				// 0xA3
	{ (uint8_t *) "movsb" },				// 0xA4
	{ (uint8_t *) "movsb", EMPTY, group_c },				// 0xA5
	{ (uint8_t *) "cmpsb" },				// 0xA6
	{ (uint8_t *) "cmpsb", EMPTY, group_d },				// 0xA7
	{ (uint8_t *) "test", (R|B) | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FO },		// 0xA8
	{ (uint8_t *) "test", (R) | (I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift },		// 0xA9
	{ (uint8_t *) "stosb" },				// 0xAA
	{ (uint8_t *) "stosb", EMPTY, group_e },				// 0xAB
	{ (uint8_t *) "lodsb" },				// 0xAC
	{ (uint8_t *) "lodsb", EMPTY, group_f },				// 0xAD
	{ (uint8_t *) "scasb" },				// 0xAE
	{ (uint8_t *) "scasb", EMPTY, group_g },				// 0xAF
	{ (uint8_t *) "mov", B | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH | FO },				// 0xB0
	{ (uint8_t *) "mov", B | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH | FO },				// 0xB1
	{ (uint8_t *) "mov", B | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH | FO },				// 0xB2
	{ (uint8_t *) "mov", B | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH | FO },				// 0xB3
	{ (uint8_t *) "mov", B | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH | FO },				// 0xB4
	{ (uint8_t *) "mov", B | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH | FO },				// 0xB5
	{ (uint8_t *) "mov", B | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH | FO },				// 0xB6
	{ (uint8_t *) "mov", B | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH | FO },				// 0xB7
	{ (uint8_t *) "mov", ((I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH	},			// 0xB8
	{ (uint8_t *) "mov", ((I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH	},				// 0xB9
	{ (uint8_t *) "mov", ((I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH	},				// 0xBA
	{ (uint8_t *) "mov", ((I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH	},				// 0xBB
	{ (uint8_t *) "mov", ((I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH	},				// 0xBC
	{ (uint8_t *) "mov", ((I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH	},				// 0xBD
	{ (uint8_t *) "mov", ((I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH	},				// 0xBE
	{ (uint8_t *) "mov", ((I) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH	},				// 0xBF
	{ EMPTY, (M|B) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FO, group_2 },				// 0xC0
	{ EMPTY, (M|D) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM, group_2 },				// 0xC1
	{ (uint8_t *) "retn", (I|W) | FO },				// 0xC2
	{ (uint8_t *) "retn" },				// 0xC3
	{ EMPTY },				// 0xC4
	{ EMPTY },				// 0xC5
	{ EMPTY, (M|B) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FO, group_3 },				// 0xC6
	{ EMPTY, (M|D) | (I|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FO, group_3 },				// 0xC7
	{ (uint8_t *) "enter", (I|W) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FO },				// 0xC8
	{ (uint8_t *) "leave" },		// 0xC9
	{ (uint8_t *) "retf", (I|W) | FO },	// 0xCA
	{ (uint8_t *) "retf" },				// 0xCB
	{ (uint8_t *) "int" },				// 0xCC
	{ (uint8_t *) "int", (I|B) | FO },				// 0xCD
	{ (uint8_t *) "into" },				// 0xCE
	{ (uint8_t *) "iretq" },				// 0xCF
	{ EMPTY, (M|B) | FM | FO, group_2 },				// 0xD0
	{ EMPTY, (M|D) | FM, group_2 },				// 0xD1
	{ EMPTY, (M|B) | FM | FO, group_2 },				// 0xD2
	{ EMPTY, (M|D) | FM, group_2 },				// 0xD3
	{ EMPTY },				// 0xD4
	{ EMPTY },				// 0xD5
	{ EMPTY },				// 0xD6
	{ EMPTY },				// 0xD7
	{ EMPTY },				// 0xD8
	{ EMPTY },				// 0xD9
	{ EMPTY },				// 0xDA
	{ EMPTY },				// 0xDB
	{ EMPTY },				// 0xDC
	{ EMPTY },				// 0xDD
	{ EMPTY },				// 0xDE
	{ EMPTY },				// 0xDF
	{ (uint8_t *) "loopnz", (I|B) | FR | FO },				// 0xE0
	{ (uint8_t *) "loopz", (I|B) | FR | FO },				// 0xE1
	{ (uint8_t *) "loop", (I|B) | FR | FO },				// 0xE2
	{ (uint8_t *) "jcxz", (I|B) | FR | FO, group_h },				// 0xE3
	{ (uint8_t *) "in", B | ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH | FO },				// 0xE4
	{ (uint8_t *) "in", ((I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift) | FH },				// 0xE5
	{ (uint8_t *) "out", FH },			// 0xE6
	{ (uint8_t *) "out", FH },				// 0xE7
	{ (uint8_t *) "call", (I) | FR },				// 0xE8
	{ (uint8_t *) "jmp", (I) | FR },				// 0xE9
	{ EMPTY },				// 0xEA
	{ (uint8_t *) "jmp", (I|B) | FO | FR },				// 0xEB
	{ (uint8_t *) "in", FH },				// 0xEC
	{ (uint8_t *) "in", FH },				// 0xED
	{ (uint8_t *) "out", FH },				// 0xEE
	{ (uint8_t *) "out", FH },				// 0xEF
	{ EMPTY },				// 0xF0
	{ EMPTY },				// 0xF1
	{ EMPTY },				// 0xF2
	{ EMPTY },				// 0xF3
	{ (uint8_t *) "hlt" },				// 0xF4
	{ (uint8_t *) "cmc" },				// 0xF5
	{ EMPTY },				// 0xF6
	{ EMPTY },				// 0xF7
	{ (uint8_t *) "clc" },				// 0xF8
	{ (uint8_t *) "stc" },				// 0xF9
	{ (uint8_t *) "cli" },				// 0xFA
	{ (uint8_t *) "sti" },				// 0xFB
	{ (uint8_t *) "cld" },				// 0xFC
	{ (uint8_t *) "std" },				// 0xFD
	{ EMPTY },				// 0xFE
	{ EMPTY }				// 0xFF
};

// struct LIB_ASM_STRUCTURE_INSTRUCTION i_0F[] = {
// };
