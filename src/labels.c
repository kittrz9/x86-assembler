#include "labels.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint32_t findLabelAddr(dynamicArray* labels, char* str) {
	for(size_t i = 0; i < labels->size; ++i) {
		struct labelStruct* l = dynamicArrayIndex(labels, i);
		if(strcmp(l->name, str) == 0) {
			return l->address;
		}
	}
	printf("could not find label \"%s\"\n", str);
	exit(1);
}

void printLabels(dynamicArray* labels) {
	for(size_t i = 0; i < labels->size; ++i) {
		struct labelStruct* l = dynamicArrayIndex(labels, i);
		printf("label %s: %lu\n", l->name, l->address);
	}
}
