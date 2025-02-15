#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "elf.h"
#include "labels.h"
#include "backpatches.h"
#include "tokens.h"

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

int main(int argc, char** argv) {
	if(argc != 2) {
		return 1;
	}
	FILE* f = fopen(argv[1], "rb");
	if(f == NULL) {
		return 1;
	}

	fseek(f, 0, SEEK_END);
	size_t fileSize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* fileBuffer = malloc(fileSize);
	fread(fileBuffer, fileSize, 1, f);
	token* t = tokenize(fileBuffer, fileSize);

	while(t->type != TOKEN_END) {
		//printf("type: %s\n", tokenNames[t->type]);
		switch(t->type) {
			case TOKEN_INSTRUCTION:
				//printf("%s\n", instrNames[t->instr]);
				switch(t->instr) {
					case INSTR_SYSCALL:
						addU8(0x0f);
						addU8(0x05);
						break;
					case INSTR_MOV:
						++t;
						if(t->type != TOKEN_REGISTER) {
							printf("expected register\n");
							exit(1);
						}
						addU8(0xb8 + t->reg);
						++t;
						if(t->type == TOKEN_ADDRESS) {
							addBackpatch(t->labelName, codePointer);
							addU32(0);
						} else if(t->type == TOKEN_INT) {
							addU32(t->intValue);
						} else {
							printf("expected address or int\n");
							exit(1);
						}
						break;
					case INSTR_JMP:
						++t;
						if(t->type != TOKEN_ADDRESS) {
							printf("expected address\n");
							exit(1);
						}
						addU8(0xe9);
						addBackpatchRelative(t->labelName, codePointer, codePointer-1);
						addU32(0);
						break;
					case INSTR_DB:
						while(1) {
							++t;
							if(t->type == TOKEN_INT) {
								addU8(t->intValue);
							} else if(t->type == TOKEN_STRING) {
								for(size_t i = 0; i < strlen(t->string); ++i) {
									addU8(t->string[i]);
								}
							} else {
								--t;
								break;
							}
						}
						break;
				}
				break;
			case TOKEN_LABEL:
				// new label
				uint8_t l = strlen(t->labelName) - 1;
				addLabel(t->labelName, l, codePointer);
				break;
			case TOKEN_ADDRESS:
				printf("%s\n", t->labelName);
				break;
			case TOKEN_INT:
				printf("%i\n", t->intValue);
				break;
			case TOKEN_REGISTER:
				printf("%s\n", regNames[t->reg]);
				break;
			case TOKEN_STRING:
				printf("%s\n", t->string);
		}
		++t;
	}

	for(size_t i = 0; i < codePointer; ++i) {
		printf("%02X", code[i]);
	}
	printf("\n");

	printLabels();

	createElfFromCode("test.elf", code, codePointer); // codePointer works as length here

	return 0;
}
