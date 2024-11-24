#include "backpatches.h"

#include <string.h>

#define MAX_BACKPATCHES 256

struct backpatchStruct backpatches[MAX_BACKPATCHES];
uint16_t backpatchPointer;

void addBackpatch(char* label, uint64_t patchAddr) {
	strcpy(backpatches[backpatchPointer].labelName, label);
	backpatches[backpatchPointer].patchAddr = patchAddr;
}
void resolveBackpatches(uint8_t* code, uint64_t virtAddr) {
	uint16_t b = 0;
	while(backpatches[b].labelName[0] != '\0') {
		uint32_t newAddr = findLabelAddr(backpatches[b].labelName) + virtAddr;
		memcpy(code+backpatches[b].patchAddr, &newAddr, sizeof(uint32_t));
		++b;
	}
}
