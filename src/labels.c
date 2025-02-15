#include "labels.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INIT_LABEL_BUFFER_SIZE 64

struct {
	struct labelStruct* buffer;
	size_t capacity;
	size_t size;
} labels;

void labelsBufferRealloc(void) {
	labels.capacity *= 2;
	labels.buffer = realloc(labels.buffer, sizeof(struct labelStruct) * labels.capacity);
}

void labelsBufferInit(void) {
	labels.buffer = malloc(sizeof(struct labelStruct) * INIT_LABEL_BUFFER_SIZE);
	labels.capacity = INIT_LABEL_BUFFER_SIZE;
	labels.size = 0;
}

void addLabel(char* labelName, uint8_t labelLen, uint64_t address) {
	memset(labels.buffer[labels.size].name, 0, MAX_LABEL_LEN);
	strncpy(labels.buffer[labels.size].name, labelName, labelLen);
	labels.buffer[labels.size].address = address;
	++labels.size;
	if(labels.size >= labels.capacity) {
		labelsBufferRealloc();
	}
}

uint32_t findLabelAddr(char* str) {
	for(size_t i = 0; i < labels.size; ++i) {
		if(strcmp(labels.buffer[i].name, str) == 0) {
			return labels.buffer[i].address;
		}
	}
	printf("could not find label \"%s\"\n", str);
	exit(1);
}

void printLabels(void) {
	for(size_t i = 0; i < labels.size; ++i) {
		printf("label %s: %lu\n", labels.buffer[i].name, labels.buffer[i].address);
	}
}
