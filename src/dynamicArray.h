#ifndef DYNAMIC_ARRAY
#define DYNAMIC_ARRAY

#include <stddef.h>

#define DYNAMIC_ARRAY_INIT_SIZE 64

typedef struct {
	void* buffer;
	size_t size;
	size_t capacity;
	size_t elementSize;
} dynamicArray;

dynamicArray* dynamicArrayCreate(size_t elementSize);
void dynamicArrayDestroy(dynamicArray* d);
void dynamicArrayAdd(dynamicArray* d, void* element);
void* dynamicArrayIndex(dynamicArray* d, size_t i);

#endif // DYNAMIC_ARRAY
