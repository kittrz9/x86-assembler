#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "elf.h"
#include "labels.h"
#include "backpatches.h"

enum x86Regs {
	REG_EAX = 0,
	REG_ECX = 1,
	REG_EDX = 2,
	REG_EBX = 3,
	REG_ESP = 4,
	REG_EBP = 5,
	REG_ESI = 6,
	REG_EDI = 7,
	REG_COUNT,
};

char regStrs[REG_COUNT][4] = {
	[REG_EAX] = "eax",
	[REG_ECX] = "ecx",
	[REG_EDX] = "edx",
	[REG_EBX] = "ebx",
	[REG_ESP] = "esp",
	[REG_EBP] = "ebp",
	[REG_ESI] = "esi",
	[REG_EDI] = "edi",
};

enum x86Regs getRegister(char* str) {
	for(uint8_t i = 0; i < REG_COUNT; ++i) {
		if(strcmp(str, regStrs[i]) == 0) {
			return i;
		}
	}
	printf("unknown register \"%s\"\n", str);
	exit(1);
}

#define MAX_LINES 128
#define MAX_STR_LEN 128

#define MAX_TOKENS 32
#define MAX_TOKEN_LEN 8

char lines[MAX_LINES][MAX_STR_LEN];

char lineTokens[MAX_LINES][MAX_TOKENS][MAX_TOKEN_LEN];

char whitespace[] = { ' ', '\t', '\n', '\0' };

bool isWhitespace(char c) {
	char* c2 = whitespace;
	while(*c2 != '\0') {
		if(*c2 == c) { return true; }
		++c2;
	}
	return false;
}

#define MAX_PROGRAM_SIZE 2048
uint8_t code[MAX_PROGRAM_SIZE];

uint64_t codePointer = 0;

void addU8(uint8_t x) {
	code[codePointer] = x;
	++codePointer;
}

void addU32(uint32_t x) {
	*(uint32_t*)&code[codePointer] = x;
	codePointer += 4;
}

char hexLUT[] = "0123456789ABCDEF";
uint32_t hexTo32(char* str) {
	size_t len = strlen(str);
	uint32_t returnVal = 0;
	for(uint8_t i = 0; i < len; ++i) {
		for(uint8_t j = 0; j < 0x10; ++j) {
			if(hexLUT[j] == str[len-i-1]) {
				returnVal |= j << i*4;
			}
		}
	}

	return returnVal;
}
uint8_t hexTo8(char* str) {
	size_t len = strlen(str);
	uint8_t returnVal = 0;
	for(uint8_t i = 0; i < len; ++i) {
		for(uint8_t j = 0; j < 0x10; ++j) {
			if(hexLUT[j] == str[len-i-1]) {
				returnVal |= j << i*4;
			}
		}
	}

	return returnVal;
}

bool endsWith(char* str, char c) {
	uint8_t l = strlen(str);
	return str[l-1] == c;
}

int main(int argc, char** argv) {
	if(argc != 2) {
		return 1;
	}
	FILE* f = fopen(argv[1], "rb");
	if(f == NULL) {
		return 1;
	}
	uint8_t lineIndex = 0;
	for(uint8_t i = 0; i < MAX_LINES; ++i) {
		char temp[MAX_STR_LEN];
		fgets(temp, MAX_STR_LEN, f);
		char* c = temp;
		bool blankLine = true;
		while(*c != '\0') {
			if(!isWhitespace(*c) && *c != ';') {
				blankLine = false;
				break;
			}
			++c;
		}
		if(!blankLine) {
			strcpy(lines[lineIndex], temp);
			++lineIndex;
		}
	}
	fclose(f);

	lineIndex = 0;
	for(uint8_t i = 0; i < MAX_LINES; ++i) {
		char* c = lines[i];
		uint8_t tokenIndex = 0;
		while(*c != '\0') {
			if(*c == ';') { break; }
			if(isWhitespace(*c)) {
				++c;
				continue;
			}
			char* nextWhitespace = c+1;
			while(!isWhitespace(*nextWhitespace)) {
				++nextWhitespace;
			}
			uint8_t tokenLen = nextWhitespace - c;
			if(tokenLen == 0) {
				exit(1);
			}
			strncpy(lineTokens[lineIndex][tokenIndex], c, tokenLen);
			printf("t:%s w:%i\n", lineTokens[lineIndex][tokenIndex], tokenLen);
			++tokenIndex;
			c += tokenLen;
		}
		if(tokenIndex != 0) {
			++lineIndex;
		}
	}

	for(uint8_t i = 0; i < MAX_LINES; ++i) {
		if(lineTokens[i][0][0] == '\0') { continue; }
		printf("line %i\n", i);
		for(uint8_t j = 0; j < MAX_TOKENS; ++j) {
			if(lineTokens[i][j][0] == '\0') { break; }
			printf("-%s\n", lineTokens[i][j]);
			if(endsWith(lineTokens[i][j], ':')) {
				// new label
				uint8_t l = strlen(lineTokens[i][j]) - 1;
				addLabel(lineTokens[i][j], l, codePointer);
			}
			if(strcmp(lineTokens[i][j], "mov") == 0) {
				++j;
				addU8(0xb8 + getRegister(lineTokens[i][j]));
				++j;
				if(lineTokens[i][j][0] == '@') {
					//addU32(findLabelAddr(lineTokens[i][j]+1));
					addBackpatch(lineTokens[i][j]+1, codePointer);
					addU32(0);
				} else {
					addU32(hexTo32(lineTokens[i][j]));
				}
			} else if(strcmp(lineTokens[i][j], "syscall") == 0) {
				addU8(0x0f);
				addU8(0x05);
			} else if(strcmp(lineTokens[i][j], "db") == 0) {
				++j;
				while(lineTokens[i][j][0] != '\0') {
					printf("%s\n", lineTokens[i][j]);
					if(lineTokens[i][j][0] == '"') {
						char* str = lineTokens[i][j] + 1;
						while(*str != '"') {
							addU8(*str);
							++str;
						}
					}
					addU8(hexTo8(lineTokens[i][j]));
					++j;
				}
			}
		}
	}

	for(size_t i = 0; i < codePointer; ++i) {
		printf("%02X", code[i]);
	}
	printf("\n");

	printLabels();

	createElfFromCode("test.elf", code, codePointer); // codePointer works as length here

	return 0;
}
