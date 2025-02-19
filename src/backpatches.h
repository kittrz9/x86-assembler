#ifndef BACKPATCHES_H
#define BACKPATCHES_H

#include <stdbool.h>

#include "labels.h"
#include "dynamicArray.h"

struct backpatchStruct {
	char labelName[MAX_LABEL_LEN];
	uint32_t patchAddr;
	size_t patchSize;
	bool relative;
	uint32_t relativeFrom;
};

void resolveBackpatches(dynamicArray* backpatches, dynamicArray* labels, uint8_t* code, uint32_t virtAddr);

#endif // BACKPATCHES_H
