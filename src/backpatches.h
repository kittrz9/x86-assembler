#ifndef BACKPATCHES_H
#define BACKPATCHES_H

#include "labels.h"

struct backpatchStruct {
	char labelName[MAX_LABEL_LEN];
	uint64_t patchAddr;
};

void addBackpatch(char* label, uint64_t patchAddr);
void resolveBackpatches(uint8_t* code, uint64_t virtAddr);

#endif // BACKPATCHES_H
