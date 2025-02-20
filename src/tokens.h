#ifndef TOKENS_H
#define TOKENS_H

#include <stdint.h>
#include <stdlib.h>

#include "dynamicArray.h"

#include "labels.h"

enum tokenType {
	TOKEN_INSTRUCTION,
	TOKEN_LABEL,
	TOKEN_ADDRESS,
	TOKEN_INT,
	TOKEN_REGISTER,
	TOKEN_STRING,
	TOKEN_DEREFERENCE,

	TOKEN_END,
};

extern char* tokenNames[];
// should move this to a better file
enum x86Regs {
	REG_EAX,
	REG_ECX,
	REG_EDX,
	REG_EBX,
	REG_ESP,
	REG_EBP,
	REG_ESI,
	REG_EDI,

	REG_COUNT,
	REG_INVALID,
};

extern char regNames[REG_COUNT][4];

// https://en.wikipedia.org/wiki/X_macro
#define INSTRUCTIONS \
	X(INSTR_MOV, mov) \
	X(INSTR_JMP, jmp) \
	X(INSTR_SYSCALL, syscall) \
	X(INSTR_DB, db) \
	X(INSTR_ADD, add) \
	X(INSTR_RET, ret) \
	X(INSTR_CALL, call) \
	X(INSTR_JZ, jz) \
	X(INSTR_JC, jc) \
	X(INSTR_JNZ, jnz) \
	X(INSTR_JNC, jnc) \
	X(INSTR_PUSH, push) \
	X(INSTR_POP, pop) \
	X(INSTR_INC, inc) \
	X(INSTR_DEC, dec) 

enum x86Instr {
#define X(a, b) a,
	INSTRUCTIONS
#undef X

	INSTR_COUNT,
	INSTR_INVALID,
};

extern char* instrNames[];

typedef struct {
	enum tokenType type;
	uint32_t lineNum;
	union {
		uint32_t intValue;
		enum x86Regs reg;
		enum x86Instr instr;
		char labelName[MAX_LABEL_LEN];
		char string[MAX_LABEL_LEN];
	};
} token;

void tokenize(dynamicArray* tokens, char* str, size_t size);

#endif // TOKENS_H
