#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "elf.h"
#include "labels.h"
#include "backpatches.h"
#include "tokens.h"

#define INIT_PROGRAM_SIZE 2048
struct {
	uint8_t* buffer;
	size_t size;
	uint64_t pointer;
} code;

void codeBufferInit(void) {
	code.buffer = malloc(INIT_PROGRAM_SIZE);
	code.size = INIT_PROGRAM_SIZE;
	code.pointer = 0;
}

void codeBufferRealloc(void) {
	code.size *= 2;
	code.buffer = realloc(code.buffer, code.size);
}

void addU8(uint8_t x) {
	code.buffer[code.pointer] = x;
	++code.pointer;
	if(code.pointer >= code.size) {
		codeBufferRealloc();
	}
}

void addU32(uint32_t x) {
	for(uint8_t i = 0; i < 4; ++i) {
		addU8(x >> i*8);
	}
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
	token* tokens = tokenize(fileBuffer, fileSize);
	token* t = tokens;

	free(fileBuffer);

	codeBufferInit();

	while(t->type != TOKEN_END) {
		switch(t->type) {
			case TOKEN_INSTRUCTION: {
				switch(t->instr) {
					case INSTR_SYSCALL: {
						addU8(0x0f);
						addU8(0x05);
						break;
					}
					case INSTR_MOV: {
						++t;
						if(t->type != TOKEN_REGISTER) {
							printf("expected register\n");
							exit(1);
						}
						addU8(0xb8 + t->reg);
						++t;
						if(t->type == TOKEN_ADDRESS) {
							addBackpatch(t->labelName, code.pointer);
							addU32(0);
						} else if(t->type == TOKEN_INT) {
							addU32(t->intValue);
						} else {
							printf("expected address or int\n");
							exit(1);
						}
						break;
					}
					case INSTR_JMP: {
						++t;
						if(t->type != TOKEN_ADDRESS) {
							printf("expected address\n");
							exit(1);
						}
						addU8(0xe9);
						addBackpatchRelative(t->labelName, code.pointer, code.pointer-1);
						addU32(0);
						break;
					}
					case INSTR_DB: {
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
					default: {
						printf("invalid instruction\n");
						exit(1);
					}
				}
				break;
			}
			case TOKEN_LABEL: {
				uint8_t l = strlen(t->labelName) - 1;
				addLabel(t->labelName, l, code.pointer);
				break;
			}
			default: {
				printf("unexpected token %s\n", tokenNames[t->type]);
				exit(1);
			}
		}
		++t;
	}

	free(tokens);

	for(size_t i = 0; i < code.pointer; ++i) {
		printf("%02X", code.buffer[i]);
	}
	printf("\n");

	printLabels();

	createElfFromCode("test.elf", code.buffer, code.pointer); // code pointer works as length here

	return 0;
}
