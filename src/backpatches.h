#ifndef BACKPATCHES_H
#define BACKPATCHES_H

#include <stdbool.h>

#include "labels.h"

struct backpatchStruct {
	char labelName[MAX_LABEL_LEN];
	uint32_t patchAddr;
	bool relative;
	uint32_t relativeFrom;
};

void backpatchBufferInit(void);
void addBackpatch(char* label, uint32_t patchAddr);
// separate function since I don't feel like changing the other lines using the function, and I think this might look a bit better than adding 2 more parameters to the first function
void addBackpatchRelative(char* label, uint32_t patchAddr, uint32_t relativeFrom);
void resolveBackpatches(uint8_t* code, uint32_t virtAddr);

#endif // BACKPATCHES_H
