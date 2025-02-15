#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "elf.h"
#include "labels.h"
#include "backpatches.h"
#include "tokens.h"

#define INIT_PROGRAM_CAPACITY 2048
struct {
	uint8_t* buffer;
	size_t capacity;
	uint64_t size;
} code;

void codeBufferInit(void) {
	code.buffer = malloc(INIT_PROGRAM_CAPACITY);
	code.capacity = INIT_PROGRAM_CAPACITY;
	code.size = 0;
}

void codeBufferRealloc(void) {
	code.capacity *= 2;
	code.buffer = realloc(code.buffer, code.capacity);
}

void addU8(uint8_t x) {
	code.buffer[code.size] = x;
	++code.size;
	if(code.size >= code.capacity) {
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
	labelsBufferInit();
	backpatchBufferInit();

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
							addBackpatch(t->labelName, code.size);
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
						addBackpatchRelative(t->labelName, code.size, code.size-1);
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
				addLabel(t->labelName, l, code.size);
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

	for(size_t i = 0; i < code.size; ++i) {
		printf("%02X", code.buffer[i]);
	}
	printf("\n");

	printLabels();

	createElfFromCode("test.elf", code.buffer, code.size);

	return 0;
}
