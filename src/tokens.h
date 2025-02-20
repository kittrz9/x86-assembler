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

enum x86Instr {
	INSTR_MOV,
	INSTR_JMP,
	INSTR_SYSCALL,
	INSTR_DB,
	INSTR_ADD,
	INSTR_RET,
	INSTR_CALL,
	INSTR_JZ,
	INSTR_JC,
	INSTR_JNZ,
	INSTR_JNC,
	INSTR_PUSH,
	INSTR_POP,
	INSTR_INC,
	INSTR_DEC,

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
