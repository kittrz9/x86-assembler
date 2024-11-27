#include "backpatches.h"

#include <string.h>
#include <stdio.h>

#define MAX_BACKPATCHES 256

struct backpatchStruct backpatches[MAX_BACKPATCHES];
uint16_t backpatchPointer;

void addBackpatch(char* label, uint32_t patchAddr) {
	strcpy(backpatches[backpatchPointer].labelName, label);
	backpatches[backpatchPointer].patchAddr = patchAddr;
	++backpatchPointer;
}

void addBackpatchRelative(char* label, uint32_t patchAddr, uint32_t relativeFrom) {
	strcpy(backpatches[backpatchPointer].labelName, label);
	backpatches[backpatchPointer].patchAddr = patchAddr;
	backpatches[backpatchPointer].relative = true;
	backpatches[backpatchPointer].relativeFrom = relativeFrom;
	++backpatchPointer;
}

void resolveBackpatches(uint8_t* code, uint32_t virtAddr) {
	uint16_t b = 0;
	while(backpatches[b].labelName[0] != '\0') {
		printf("%s\n", backpatches[b].labelName);
		if(backpatches[b].relative) {
			int32_t relative = findLabelAddr(backpatches[b].labelName) - backpatches[b].relativeFrom - 5;
			printf("%i\n", relative);
			memcpy(code+backpatches[b].patchAddr, &relative, sizeof(uint32_t));
		} else {
			uint32_t newAddr = findLabelAddr(backpatches[b].labelName) + virtAddr;
			memcpy(code+backpatches[b].patchAddr, &newAddr, sizeof(uint32_t));
		}
		++b;
	}
}
