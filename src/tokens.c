#include "tokens.h"

#include <stdio.h>
#include <string.h>

char* tokenNames[] = {
	[TOKEN_INSTRUCTION] = "TOKEN_INSTRUCTION",
	[TOKEN_LABEL] = "TOKEN_LABEL",
	[TOKEN_ADDRESS] = "TOKEN_ADDRESS",
	[TOKEN_INT] = "TOKEN_INT",
	[TOKEN_REGISTER] = "TOKEN_REGISTER",
	[TOKEN_STRING] = "TOKEN_STRING",
	[TOKEN_DEREFERENCE] = "TOKEN_DEREFERENCE",
	[TOKEN_END] = "TOKEN_END",
};

uint8_t isWhitespace(char c) {
	return (c==' ')||(c=='\n')||(c=='\t')||(c=='\0');
}

char hexLUT[] = "0123456789ABCDEF";
uint32_t hexTo32(char* str) {
	size_t len = strlen(str);
	uint32_t returnVal = 0;
	for(uint8_t i = 0; i < len; ++i) {
		for(uint8_t j = 0; j < 0x10; ++j) {
			char c = str[len-i-1];
			if(c >= 'a' && c <= 'z') {
				c -= 'a'-'A';
			}
			if(hexLUT[j] == c) {
				returnVal |= j << i*4;
			}
		}
	}

	return returnVal;
}

uint8_t endsWith(char* str, char c) {
	uint8_t l = strlen(str);
	if(l == 0) { return 0; }
	return str[l-1] == c;
}

#define MAX_TOKEN_LEN 64
void strAppend(char* str, char c) {
	size_t l = strlen(str);
	if(l >= MAX_TOKEN_LEN-1) {
		printf("max token length exceeded\n");
		exit(1);
	}
	str[l] = c;
	str[l+1] = '\0';
}

char* instrNames[] = {
	[INSTR_MOV] = "mov",
	[INSTR_JMP] = "jmp",
	[INSTR_SYSCALL] = "syscall",
	[INSTR_DB] = "db",
	[INSTR_ADD] = "add",
};

enum x86Instr isValidInstruction(char* str) {
	for(size_t i = 0; i < INSTR_COUNT; ++i) {
		if(strcmp(instrNames[i], str) == 0) {
			return i;
		}
	}
	return INSTR_INVALID;
}


char regNames[REG_COUNT][4] = {
	[REG_EAX] = "eax",
	[REG_ECX] = "ecx",
	[REG_EDX] = "edx",
	[REG_EBX] = "ebx",
	[REG_ESP] = "esp",
	[REG_EBP] = "ebp",
	[REG_ESI] = "esi",
	[REG_EDI] = "edi",
};

enum x86Regs isValidRegister(char* str) {
	for(size_t i = 0; i < REG_COUNT; ++i) {
		if(strcmp(regNames[i], str) == 0) {
			return i;
		}
	}
	return REG_INVALID;
}

void tokenize(dynamicArray* tokens, char* str, size_t size) {
	char tokenStr[MAX_TOKEN_LEN];
	memset(tokenStr, 0, MAX_TOKEN_LEN);
	token t;
	for(size_t i = 0; i < size; ++i) {
		char c = str[i];
		if(c == ';') {
			do {
				++i;
				c = str[i];
			} while(c != '\n');
		}
		if(c == '"') {
			t.type = TOKEN_STRING;
			t.string[0] = '\0';
			++i;
			c = str[i];
			while(c != '"') {
				strAppend(t.string, c);
				++i;
				c = str[i];
			}
			++i;
			dynamicArrayAdd(tokens, &t);
			continue;
		}
		if(c == '*') {
			t.type = TOKEN_DEREFERENCE;
			dynamicArrayAdd(tokens, &t);
			continue;
		}
		if(isWhitespace(c)) {
			enum x86Regs r = isValidRegister(tokenStr);
			enum x86Instr instr = isValidInstruction(tokenStr);
			if(r != REG_INVALID) {
				t.type = TOKEN_REGISTER;
				t.reg = r;
				dynamicArrayAdd(tokens, &t);
			} else if(instr != INSTR_INVALID) {
				t.type = TOKEN_INSTRUCTION;
				t.instr = instr;
				dynamicArrayAdd(tokens, &t);
			} else if(tokenStr[0] == '#') {
				t.type = TOKEN_INT;
				t.intValue = hexTo32(tokenStr+1);
				dynamicArrayAdd(tokens, &t);
			} else if(endsWith(tokenStr, ':')) {
				t.type = TOKEN_LABEL;
				strcpy(t.labelName, tokenStr);
				dynamicArrayAdd(tokens, &t);
			} else if(tokenStr[0] != '\0') { // treat any other string as a label address
				t.type = TOKEN_ADDRESS;
				strcpy(t.labelName, tokenStr);
				dynamicArrayAdd(tokens, &t);
			}


			tokenStr[0] = '\0';
			continue;
		} else {
			strAppend(tokenStr, c);
		}
	}
	t.type = TOKEN_END;
	dynamicArrayAdd(tokens, &t);
	return;
}

