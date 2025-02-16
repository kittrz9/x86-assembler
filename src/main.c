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
							if(t->type != TOKEN_REGISTER) {
								printf("expected register\n");
								exit(1);
							}
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
							if(t->type != TOKEN_REGISTER) {
								printf("expected register\n");
								exit(1);
							}
							if((t+1)->type == TOKEN_REGISTER) {
								++t;
								addU8(code, 0x89);
								addU8(code, 0xc0 + t->reg * 8);
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
						printf("invalid instruction\n");
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
				printf("unexpected token %s\n", tokenNames[t->type]);
				exit(1);
			}
		}
		++t;
	}

	free(tokens);

	for(size_t i = 0; i < code->size; ++i) {
		printf("%02X", *(uint8_t*)dynamicArrayIndex(code, i));
	}
	printf("\n");

	printLabels(labels);

	createElfFromCode("test.elf", code, labels, backpatches);

	dynamicArrayDestroy(code);
	dynamicArrayDestroy(labels);
	dynamicArrayDestroy(backpatches);

	return 0;
}
