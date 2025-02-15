#include "backpatches.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define INIT_BACKPATCH_BUFFER_SIZE 256

struct {
	struct backpatchStruct* buffer;
	size_t capacity;
	size_t size;
} backpatches;

void backpatchBufferRealloc(void) {
	backpatches.capacity *= 2;
	backpatches.buffer = realloc(backpatches.buffer, sizeof(struct labelStruct) * backpatches.capacity);
}

void backpatchBufferInit(void) {
	backpatches.buffer = malloc(sizeof(struct labelStruct) * INIT_BACKPATCH_BUFFER_SIZE);
	backpatches.capacity = INIT_BACKPATCH_BUFFER_SIZE;
	backpatches.size = 0;
}

void addBackpatch(char* label, uint32_t patchAddr) {
	memset(backpatches.buffer[backpatches.size].labelName, 0, MAX_LABEL_LEN);
	strcpy(backpatches.buffer[backpatches.size].labelName, label);
	backpatches.buffer[backpatches.size].relative = false;
	backpatches.buffer[backpatches.size].patchAddr = patchAddr;
	++backpatches.size;
	if(backpatches.size >= backpatches.capacity) {
		backpatchBufferRealloc();
	}
}

void addBackpatchRelative(char* label, uint32_t patchAddr, uint32_t relativeFrom) {
	strcpy(backpatches.buffer[backpatches.size].labelName, label);
	backpatches.buffer[backpatches.size].patchAddr = patchAddr;
	backpatches.buffer[backpatches.size].relative = true;
	backpatches.buffer[backpatches.size].relativeFrom = relativeFrom;
	++backpatches.size;
}

void resolveBackpatches(uint8_t* code, uint32_t virtAddr) {
	for(size_t i = 0; i < backpatches.size; ++i) {
		printf("%s\n", backpatches.buffer[i].labelName);
		if(backpatches.buffer[i].relative) {
			int32_t relative = findLabelAddr(backpatches.buffer[i].labelName) - backpatches.buffer[i].relativeFrom - 5;
			printf("%i\n", relative);
			memcpy(code+backpatches.buffer[i].patchAddr, &relative, sizeof(uint32_t));
		} else {
			uint32_t newAddr = findLabelAddr(backpatches.buffer[i].labelName) + virtAddr;
			memcpy(code+backpatches.buffer[i].patchAddr, &newAddr, sizeof(uint32_t));
		}
	}
}
