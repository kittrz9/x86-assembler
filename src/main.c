#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "elf.h"
#include "labels.h"
#include "backpatches.h"
#include "tokens.h"
#include "dynamicArray.h"

void addU8(dynamicArray* code, uint8_t x) {
	dynamicArrayAdd(code, &x);
}

void addU32(dynamicArray* code, uint32_t x) {
	for(uint8_t i = 0; i < 4; ++i) {
		addU8(code, x >> i*8);
	}
}

void lineError(char* str, uint32_t lineNum) {
	printf("line %i: %s\n", lineNum, str);
	exit(1);
}

void expect(enum tokenType expectedToken, token* newToken) {
	if(newToken->type != expectedToken) {
		char str[64];
		snprintf(str, 64, "expected %s, got %s", tokenNames[expectedToken], tokenNames[newToken->type]);
		lineError(str, newToken->lineNum);
	}
	return;
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
	dynamicArray* tokens = dynamicArrayCreate(sizeof(token));
	tokenize(tokens, fileBuffer, fileSize);
	token* t = tokens->buffer;

	free(fileBuffer);

	dynamicArray* code = dynamicArrayCreate(1);
	dynamicArray* labels = dynamicArrayCreate(sizeof(struct labelStruct));
	dynamicArray* backpatches = dynamicArrayCreate(sizeof(struct backpatchStruct));

	while(t->type != TOKEN_END) {
		switch(t->type) {
			case TOKEN_INSTRUCTION: {
				switch(t->instr) {
					case INSTR_SYSCALL: {
						addU8(code, 0x0f);
						addU8(code, 0x05);
						break;
					}
					case INSTR_MOV: {
						++t;
						if(t->type == TOKEN_DEREFERENCE) {
							++t;
							expect(TOKEN_REGISTER, t);
							if((t+1)->type == TOKEN_REGISTER) { // this is a mess
								++t;
								addU8(code, 0x89);
								addU8(code, t->reg * 8);
								break;
							} else {
								addU8(code, 0x67); // probably not necessary, but the address size override prefix makes it use eax instead of rax I think (according to objdump)
								addU8(code, 0xc7);
								addU8(code, t->reg);
							}
						} else {
							expect(TOKEN_REGISTER, t);
							if((t+1)->type == TOKEN_REGISTER) {
								++t;
								addU8(code, 0x89);
								addU8(code, 0xc0 + t->reg * 8);
								break;
							} else if((t+1)->type == TOKEN_DEREFERENCE) {
								uint8_t dst = t->reg * 8;
								++t;
								++t;
								expect(TOKEN_REGISTER, t);
								addU8(code, 0x8b);
								addU8(code, t->reg + dst);
								break;
							} else {
								addU8(code, 0xb8 + t->reg);
							}
						}
						++t;
						if(t->type == TOKEN_ADDRESS) {
							struct backpatchStruct b;
							b.relative = false;
							b.patchAddr = code->size;
							strncpy(b.labelName, t->labelName, MAX_LABEL_LEN);
							dynamicArrayAdd(backpatches, &b);
							//addBackpatch(backpatches, t->labelName, code->size);
							addU32(code, 0);
						} else if(t->type == TOKEN_INT) {
							addU32(code, t->intValue);
						} else {
							lineError("expected address or int", t->lineNum);
							exit(1);
						}
						break;
					}
					case INSTR_JMP: {
						++t;
						expect(TOKEN_ADDRESS, t);
						addU8(code, 0xe9);
						struct backpatchStruct b;
						b.relative = true;
						b.relativeFrom = code->size-1;
						b.patchAddr = code->size;
						strncpy(b.labelName, t->labelName, MAX_LABEL_LEN);
						dynamicArrayAdd(backpatches, &b);
						//addBackpatchRelative(backpatches, t->labelName, code->size, code->size-1);
						addU32(code, 0);
						break;
					}
					case INSTR_ADD: {
						++t;
						expect(TOKEN_REGISTER, t);
						addU8(code, 0x81);
						addU8(code, 0xc0 + t->reg);
						++t;
						addU32(code, t->intValue);
						break;
					}
					case INSTR_DB: {
						while(1) {
							++t;
							if(t->type == TOKEN_INT) {
								addU8(code, t->intValue);
							} else if(t->type == TOKEN_STRING) {
								for(size_t i = 0; i < strlen(t->string); ++i) {
									addU8(code, t->string[i]);
								}
							} else {
								--t;
								break;
							}
						}
						break;
					}
					default: {
						lineError("invalid instruction", t->lineNum);
						exit(1);
					}
				}
				break;
			}
			case TOKEN_LABEL: {
				struct labelStruct l;
				l.address = code->size;
				strncpy(l.name, t->labelName, strlen(t->labelName)-1); // removes the : at the end
				l.name[strlen(t->labelName)-1] = '\0';
				dynamicArrayAdd(labels, &l);
				break;
			}
			default: {
				lineError("unexpected token\n", t->lineNum);
				exit(1);
			}
		}
		++t;
	}

	for(size_t i = 0; i < code->size; ++i) {
		printf("%02X", *(uint8_t*)dynamicArrayIndex(code, i));
	}
	printf("\n");

	printLabels(labels);

	createElfFromCode("test.elf", code, labels, backpatches);

	dynamicArrayDestroy(code);
	dynamicArrayDestroy(labels);
	dynamicArrayDestroy(backpatches);
	dynamicArrayDestroy(tokens);

	return 0;
}
