#ifndef TOKENS_H
#define TOKENS_H

#include <stdint.h>
#include <stdlib.h>

enum tokenType {
	TOKEN_INSTRUCTION,
	TOKEN_LABEL,
	TOKEN_ADDRESS,
	TOKEN_INT,
	TOKEN_REGISTER,
	TOKEN_STRING,
	TOKEN_END,
};

extern char* tokenNames[];
// should move this to a better file
enum x86Regs {
	REG_NULL, // to avoid returning 0 when checking the array of register names
	REG_EAX,
	REG_ECX,
	REG_EDX,
	REG_EBX,
	REG_ESP,
	REG_EBP,
	REG_ESI,
	REG_EDI,
	REG_COUNT,
};
extern char regStrs[REG_COUNT][4];

typedef struct {
	enum tokenType type;
	union {
		uint32_t intValue;
		enum x86Regs reg;
		char instructionName[32]; // will eventually probably be an enum
		char labelName[32];
		char string[32];
	};
} token;

token* tokenize(char* str, size_t size);

#endif // TOKENS_H
