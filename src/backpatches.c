#include "backpatches.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "endian.h"

void resolveBackpatches(dynamicArray* backpatches, dynamicArray* labels, uint8_t* code, uint32_t virtAddr) {
	for(size_t i = 0; i < backpatches->size; ++i) {
		struct backpatchStruct* b = dynamicArrayIndex(backpatches, i);
		printf("%s\n", b->labelName);
		if(b->relative) {
			int32_t relative = findLabelAddr(labels, b->labelName) - b->relativeFrom - 5;
			endianSwap(&relative, sizeof(relative), LITTLE_ENDIAN);
			printf("%i\n", relative);
			memcpy(code+b->patchAddr, &relative, sizeof(uint32_t));
		} else {
			uint32_t newAddr = findLabelAddr(labels, b->labelName) + virtAddr;
			endianSwap(&newAddr, sizeof(newAddr), LITTLE_ENDIAN);
			memcpy(code+b->patchAddr, &newAddr, sizeof(uint32_t));
		}
	}
}
