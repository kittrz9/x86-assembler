#include "labels.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct labelStruct labels[MAX_LABELS];
uint8_t labelPointer;

void addLabel(char* labelName, uint8_t labelLen, uint64_t address) {
	strncpy(labels[labelPointer].name, labelName, labelLen);
	labels[labelPointer].address = address;
	++labelPointer;
}

uint32_t findLabelAddr(char* str) {
	uint8_t l = 0;
	while(labels[l].name[0] != '\0') {
		if(strcmp(labels[l].name, str) == 0) {
			return labels[l].address;
		}
		++l;
	}
	printf("could not find label \"%s\"\n", str);
	exit(1);
}

void printLabels(void) {
	uint8_t l = 0;
	while(labels[l].name[0] != '\0') {
		printf("label %s: %lu\n", labels[l].name, labels[l].address);
		++l;
	}
}
