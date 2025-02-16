#include "dynamicArray.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

dynamicArray* dynamicArrayCreate(size_t elementSize) {
	dynamicArray* d = malloc(sizeof(dynamicArray));
	d->buffer = malloc(elementSize * DYNAMIC_ARRAY_INIT_SIZE);
	d->capacity = DYNAMIC_ARRAY_INIT_SIZE;
	d->size = 0;
	d->elementSize = elementSize;
	return d;
}

void dynamicArrayDestroy(dynamicArray* d) {
	free(d->buffer);
	free(d);
}

void dynamicArrayAdd(dynamicArray* d, void* element) {
	memset((uint8_t*)d->buffer + (d->size * d->elementSize), 0, d->elementSize);
	memcpy((uint8_t*)d->buffer + (d->size * d->elementSize), element, d->elementSize);
	++d->size;
	if(d->size >= d->capacity) {
		d->capacity *= 2;
		d->buffer = realloc(d->buffer, d->elementSize * d->capacity);
	}
}

void* dynamicArrayIndex(dynamicArray* d, size_t i) {
	return (uint8_t*)d->buffer + (d->elementSize * i);
}

