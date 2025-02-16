#ifndef LABELS_H
#define LABELS_H

#include <stdint.h>

#include "dynamicArray.h"

#define MAX_LABEL_LEN 32

struct labelStruct {
	char name[MAX_LABEL_LEN];
	uint64_t address;
};

uint32_t findLabelAddr(dynamicArray* labels, char* str);
void printLabels(dynamicArray* labels);

#endif // LABELS_H
