#ifndef LABELS_H
#define LABELS_H

#include <stdint.h>

#define MAX_LABEL_LEN 32
#define MAX_LABELS 64

struct labelStruct {
	char name[MAX_LABEL_LEN];
	uint64_t address;
};

void labelsBufferInit(void);
void addLabel(char* labelName, uint8_t labelLen, uint64_t address);
uint32_t findLabelAddr(char* str);
void printLabels(void);

#endif // LABELS_H
